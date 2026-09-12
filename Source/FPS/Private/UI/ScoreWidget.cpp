// Copyright Fabian Fröding.

#include "UI/ScoreWidget.h"

#include "Components/TextBlock.h"
#include "Player/ShooterPlayerController.h"
#include "Player/ShooterPlayerState.h"

class AShooterPlayerController;

void UScoreWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (AShooterPlayerState* PS = GetPlayerState(); IsValid(PS))
	{
		PS->OnScoreChanged.AddDynamic(this, &ThisClass::OnScoreChanged);
	}
	else
	{
		if (AShooterPlayerController* PC = Cast<AShooterPlayerController>(GetOwningPlayer()); IsValid(PC))
		{
			PC->OnPlayerStateReplicated.AddUniqueDynamic(this, &ThisClass::OnPlayerStateReplicated);
		}
	}
}

AShooterPlayerState* UScoreWidget::GetPlayerState() const
{
	if (APlayerController* PC = GetOwningPlayer(); IsValid(PC))
	{
		return PC->GetPlayerState<AShooterPlayerState>();
	}
	return nullptr;
}

void UScoreWidget::OnScoreChanged(const int32 Score)
{
	if (IsValid(Text_Score))
	{
		Text_Score->SetText(FText::AsNumber(Score));
	}
}

void UScoreWidget::OnPlayerStateReplicated()
{
	if (AShooterPlayerState* PS = GetPlayerState(); IsValid(PS))
	{
		PS->OnScoreChanged.AddDynamic(this, &ThisClass::OnScoreChanged);
		OnScoreChanged(PS->GetScoredElims());
	}

	if (AShooterPlayerController* PC = Cast<AShooterPlayerController>(GetOwningPlayer()); IsValid(PC))
	{
		PC->OnPlayerStateReplicated.RemoveDynamic(this, &ThisClass::OnPlayerStateReplicated);
	}
}
