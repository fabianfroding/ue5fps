// Copyright Fabian Fröding.

#include "Elimination/EliminationComponent.h"

#include "Game/ShooterGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ShooterPlayerState.h"
#include "ShooterTypes/ShooterTypes.h"

UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SequentialElimInterval = 2.0f;
	LastElimTime = 0.0f;
	SequentialElims = 0;
	Streak = 0;
	ElimsNeededForStreak = 5;
}

AShooterPlayerState* UEliminationComponent::GetPlayerStateFromActor(AActor* Actor)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (IsValid(Pawn))
	{
		return Pawn->GetPlayerState<AShooterPlayerState>();
	}
	return nullptr;
}

void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	UE_LOG(LogTemp, Warning, TEXT("UEliminationComponent::OnRoundReported: Hit: %d. HeadShot: %d. Lethal: %d."), bHit, bHeadShot, bLethal);
	AShooterPlayerState* AttackerPS = GetPlayerStateFromActor(Attacker);
	if (!IsValid(AttackerPS)) return;
	ProcessHitOrMiss(bHit, AttackerPS);
	
	if (!bHit) return;
	
	AShooterPlayerState* VictimPS = GetPlayerStateFromActor(Victim);
	if (!IsValid(VictimPS)) return;
	
	if (bLethal)
	{
		ProcessElimination(bHeadShot, AttackerPS, VictimPS);
	}
}

void UEliminationComponent::ProcessHitOrMiss(bool bHit, AShooterPlayerState* AttackerPS)
{
	if (bHit)
	{
		AttackerPS->AddHit();
	}
	else
	{
		AttackerPS->AddMiss();
	}
}

void UEliminationComponent::ProcessElimination(bool bHeadShot, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS)
{
	AttackerPS->AddScoredElim();
	VictimPS->AddDefeat();
	
	ESpecialElimType SpecialElimType{};
	
	ProcessHeadShot(bHeadShot, SpecialElimType, AttackerPS);
	ProcessSequentialEliminations(SpecialElimType, AttackerPS);
	ProcessStreaks(SpecialElimType, AttackerPS, VictimPS);
	
	AShooterGameStateBase* GameState = Cast<AShooterGameStateBase>(UGameplayStatics::GetGameState(AttackerPS));
	if (IsValid(GameState))
	{
		HandleFirstBlood(GameState, SpecialElimType, AttackerPS);
	}
	
	UpdateLeaderStatus(GameState, SpecialElimType, AttackerPS, VictimPS);
	
	// If has special elim types - Tell client which special elims we got
	// Else we just got a regular elim
}

void UEliminationComponent::ProcessHeadShot(bool bHeadShot, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	if (bHeadShot)
	{
		OutElimType |= ESpecialElimType::HeadShot; // Shorthand for OutElimType = OutElimType | ESpecialElimType::HeadShot;
		AttackerPS->AddHeadShotElim();
	}
}

void UEliminationComponent::ProcessSequentialEliminations(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastElimTime <= SequentialElimInterval)
	{
		++SequentialElims;
	}
	else
	{
		SequentialElims = 1;
	}
	LastElimTime = CurrentTime;
	
	if (SequentialElims > 1)
	{
		OutElimType |= ESpecialElimType::Sequential;
		AttackerPS->AddSequentialElim(SequentialElims);
	}
}

void UEliminationComponent::ProcessStreaks(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS)
{
	++Streak;
	if (Streak >= ElimsNeededForStreak)
	{
		OutElimType |= ESpecialElimType::Streak;
		AttackerPS->SetOnStreak(true);
		AttackerPS->UpdateHighestStreak(Streak);
	}
	if (VictimPS->IsOnStreak())
	{
		OutElimType |= ESpecialElimType::Showstopper;
		AttackerPS->AddShowStopperElim();
		VictimPS->SetOnStreak(false);
	}
	if (AttackerPS->GetLastAttacker() == VictimPS)
	{
		OutElimType |= ESpecialElimType::Revenge;
		AttackerPS->AddRevengeElim();
		AttackerPS->SetLastAttacker(nullptr);
	}
	VictimPS->SetLastAttacker(AttackerPS);
}

void UEliminationComponent::HandleFirstBlood(AShooterGameStateBase* GameState, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	if (!GameState->HasFirstBloodBeenHad())
	{
		OutElimType |= ESpecialElimType::FirstBlood;
		AttackerPS->GotFirstBlood();
	}
}

void UEliminationComponent::UpdateLeaderStatus(AShooterGameStateBase* GameState, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS)
{
	AShooterPlayerState* LastLeader = GameState->GetSoleLeader();
	const bool bAttackerWasTiedForTheLead = GameState->IsTiedForTheLead(AttackerPS);
	GameState->UpdateLeader();
	
	if (!bAttackerWasTiedForTheLead && GameState->IsTiedForTheLead(AttackerPS))
	{
		OutElimType |= ESpecialElimType::TiedTheLeader; // Attacker not tied for the lead before but is now after sorting.
	}
	
	if (IsValid(LastLeader) && LastLeader != GameState->GetSoleLeader())
	{
		LastLeader->Client_LostTheLead(); // Last leader lost the lead.
		
		if (VictimPS == LastLeader)
		{
			OutElimType |= ESpecialElimType::Dethrone;
			AttackerPS->AddDethroneElim();
		}
	}
	
	if (AttackerPS != LastLeader && AttackerPS == GameState->GetSoleLeader())
	{
		OutElimType |= ESpecialElimType::GainedTheLead;
	}
}
