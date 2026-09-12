// Copyright Fabian Fröding.

#include "Player/ShooterPlayerState.h"

#include "Data/SpecialElimData.h"
#include "UI/SpecialElimWidget.h"


AShooterPlayerState::AShooterPlayerState()
{
	SetNetUpdateFrequency(100.f);
	
	ScoredElims = 0;
	Defeats = 0;
	Hits = 0;
	Misses = 0;
	bOnStreak = false;
	HeadShotElims = 0;
	HighestStreak = 0;
	RevengeElims = 0;
	DethroneElims = 0;
	ShowStopperElims = 0;
	bFirstBlood = false;
	bWinner = false;
	bIsProcessingQueue = false;
	ElimDisplayTime = 0.5f;
}

void AShooterPlayerState::AddScoredElim()
{
	++ScoredElims;
}

void AShooterPlayerState::AddDefeat()
{
	++Defeats;
}

void AShooterPlayerState::AddHit()
{
	++Hits;
}

void AShooterPlayerState::AddMiss()
{
	++Misses;
}

void AShooterPlayerState::AddHeadShotElim()
{
	++HeadShotElims;
}

void AShooterPlayerState::AddSequentialElim(int32 SequenceCount)
{
	if (SequentialElims.Contains(SequenceCount))
	{
		SequentialElims[SequenceCount]++;
	}
	else
	{
		SequentialElims.Add(SequenceCount, 1);
	}
	
	// Logic to prevent a higher sequential elim to count as a lower sequential elim (triple also counting as double etc).
	for (auto& Elem : SequentialElims)
	{
		if (Elem.Key < SequenceCount && Elem.Value > 0)
		{
			Elem.Value--;
		}
	}
}

void AShooterPlayerState::UpdateHighestStreak(int32 StreakCount)
{
	if (StreakCount > HighestStreak)
	{
		HighestStreak = StreakCount;
	}
}

void AShooterPlayerState::AddRevengeElim()
{
	++RevengeElims;
}

void AShooterPlayerState::AddDethroneElim()
{
	++DethroneElims;
}

void AShooterPlayerState::AddShowStopperElim()
{
	++ShowStopperElims;
}

void AShooterPlayerState::GotFirstBlood()
{
	bFirstBlood = true;
}

void AShooterPlayerState::IsNowWinner()
{
	bWinner = true;
}

void AShooterPlayerState::SetOnStreak(bool bIsOnStreak)
{
	bOnStreak = bIsOnStreak;
}

bool AShooterPlayerState::IsOnStreak() const
{
	return bOnStreak;
}

void AShooterPlayerState::SetLastAttacker(APlayerState* Attacker)
{
	LastAttacker = Attacker;
}

APlayerState* AShooterPlayerState::GetLastAttacker() const
{
	return LastAttacker.IsValid() ? LastAttacker.Get() : nullptr;
}

int32 AShooterPlayerState::GetScoredElims() const
{
	return ScoredElims;
}

TArray<ESpecialElimType> AShooterPlayerState::DecodeElimBitmask(ESpecialElimType ElimTypeBitmask)
{
	TArray<ESpecialElimType> ValidElims;
	
	uint16 BitmaskValue = static_cast<uint16>(ElimTypeBitmask);
	
	for (int16 i = 0; i < 16; i++)
	{
		// 00000000		&	 00000001 etc -> false. Both need 1 at same position.
		if (BitmaskValue & (1 << i))
		{
			ESpecialElimType EnumValue = static_cast<ESpecialElimType>(1 << i);
			ValidElims.Add(EnumValue);
		}
	}
	
	return ValidElims;
}

void AShooterPlayerState::Client_SpecialElim_Implementation(const ESpecialElimType& SpecialElim, int32 SequentialElimCount, int32 StreakCount, int32 ElimScore)
{
	ensure(IsValid(SpecialElimData));
	
	OnScoreChanged.Broadcast(ElimScore);
	
	TArray<ESpecialElimType> ElimTypes = DecodeElimBitmask(SpecialElim);
	for (ESpecialElimType ElimType : ElimTypes)
	{
		FSpecialElimInfo& ElimMessageInfo = SpecialElimData->SpecialElimInfo.FindChecked(ElimType);
		if (ElimType == ESpecialElimType::Sequential)
		{
			ElimMessageInfo.SequentialElimCount = SequentialElimCount;
		}
		if (ElimType == ESpecialElimType::Streak)
		{
			ElimMessageInfo.StreakCount = StreakCount;
		}
		ElimMessageInfo.ElimType = ElimType;
		
		// Queue message info to process them over time.
		SpecialElimQueue.Enqueue(ElimMessageInfo);
	}
	
	if (!bIsProcessingQueue)
	{
		// Process next item in the queue
		ProcessNextSpecialElim();
	}
}

void AShooterPlayerState::Client_ScoredElim_Implementation(int32 ElimScore)
{
	OnScoreChanged.Broadcast(ElimScore);
}

void AShooterPlayerState::Client_LostTheLead_Implementation()
{
	ensure(IsValid(SpecialElimData));
	const FSpecialElimInfo& ElimMessageInfo = SpecialElimData->SpecialElimInfo.FindChecked(ESpecialElimType::LostTheLead);
	
	if (IsValid(SpecialElimWidgetClass))
	{
		USpecialElimWidget* SpecialElimWidget = CreateWidget<USpecialElimWidget>(GetPlayerController(), SpecialElimWidgetClass);
		if (IsValid(SpecialElimWidget))
		{
			SpecialElimWidget->InitializeWidget(ElimMessageInfo.ElimMessage.ToString(), ElimMessageInfo.ElimIcon);
			SpecialElimWidget->AddToViewport();
		}
	}
}

void AShooterPlayerState::ProcessNextSpecialElim()
{
	FSpecialElimInfo ElimInfo;
	if (SpecialElimQueue.Dequeue(ElimInfo))
	{
		bIsProcessingQueue = true;
		ShowSpecialElim(ElimInfo);
		
		// Wait until next tick, then wait for the display time before processing next elim.
		GetWorldTimerManager().SetTimerForNextTick([this]()
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterPlayerState::ProcessNextSpecialElim, ElimDisplayTime, false);
		});
	}
	else
	{
		bIsProcessingQueue = false;
	}
}

void AShooterPlayerState::ShowSpecialElim(const FSpecialElimInfo& ElimMessageInfo)
{
	FString ElimMessageString = ElimMessageInfo.ElimMessage.ToString();
	if (ElimMessageInfo.ElimType == ESpecialElimType::Sequential)
	{
		if (ElimMessageInfo.SequentialElimCount == 2) ElimMessageString = FString("Double kill!");
		else if (ElimMessageInfo.SequentialElimCount == 3) ElimMessageString = FString("Triple kill!");
		else if (ElimMessageInfo.SequentialElimCount == 4) ElimMessageString = FString("Quadra kill!");
		else if (ElimMessageInfo.SequentialElimCount > 4) ElimMessageString = FString::Printf(TEXT("Rampage x%d!"), ElimMessageInfo.SequentialElimCount);
	}
	if (ElimMessageInfo.ElimType == ESpecialElimType::Streak)
	{
		ElimMessageString = FString::Printf(TEXT("Streak x%d!"), ElimMessageInfo.StreakCount);
	}
	
	if (IsValid(SpecialElimWidgetClass))
	{
		USpecialElimWidget* SpecialElimWidget = CreateWidget<USpecialElimWidget>(GetPlayerController(), SpecialElimWidgetClass);
		if (IsValid(SpecialElimWidget))
		{
			SpecialElimWidget->InitializeWidget(ElimMessageString, ElimMessageInfo.ElimIcon);
			SpecialElimWidget->AddToViewport();
		}
	}
}
