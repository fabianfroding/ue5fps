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
	
}

void UCombatComponent::InitiateFireWeaponPressed()
{
	
}

void UCombatComponent::InitiateFireWeaponReleased()
{
	
}

void UCombatComponent::InitiateReloadWeapon()
{
	
}

void UCombatComponent::InitiateAimPressed()
{
	
}

void UCombatComponent::InitiateAimReleased()
{
	
}
