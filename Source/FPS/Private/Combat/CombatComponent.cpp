// Copyright Fabian Fröding.

#include "Combat/CombatComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::InitiateCycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("InitiateCycleWeapon"), false);
}

void UCombatComponent::InitiateFireWeaponPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("InitiateFireWeaponPressed"), false);
}

void UCombatComponent::InitiateFireWeaponReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("InitiateFireWeaponReleased"), false);
}

void UCombatComponent::InitiateReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("InitiateReloadWeapon"), false);
}

void UCombatComponent::InitiateAimPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("InitiateAimPressed"), false);
}

void UCombatComponent::InitiateAimReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("InitiateAimReleased"), false);
}
