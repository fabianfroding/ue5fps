// Copyright Fabian Fröding.

#include "Elimination/EliminationComponent.h"

UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	UE_LOG(LogTemp, Warning, TEXT("UEliminationComponent::OnRoundReported: Hit: %d. HeadShot: %d. Lethal: %d."), bHit, bHeadShot, bLethal);
}
