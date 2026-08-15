// Copyright Fabian Fröding.

#include "UI/ShooterReticle.h"

#include "Characters/ShooterCharacter.h"
#include "Combat/CombatComponent.h"
#include "Weapon/Weapon.h"


void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	// Pattern to deal with race-condition between pawn possessed and weapon replicated.
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		if (AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter); IsValid(Weapon))
		{
			OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance());
			OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombatComponent = UCombatComponent::FindCombatComponent(OldPawn);
	UCombatComponent* NewPawnCombatComponent = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(OldPawnCombatComponent))
	{
		OldPawnCombatComponent->OnReticleChanged.RemoveDynamic(this, &ThisClass::OnReticleChanged);
		OldPawnCombatComponent->OnAmmoCounterChanged.RemoveDynamic(this, &ThisClass::OnAmmoCounterChanged);
	}
	if (IsValid(NewPawnCombatComponent))
	{
		NewPawnCombatComponent->OnReticleChanged.AddDynamic(this, &ThisClass::OnReticleChanged);
		NewPawnCombatComponent->OnAmmoCounterChanged.AddDynamic(this, &ThisClass::OnAmmoCounterChanged);
	}
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	if (!IsValid(Weapon)) return;
	OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance());
	OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
}

void UShooterReticle::OnReticleChanged(UMaterialInstanceDynamic* ReticleDynMatInst)
{
	// Set the material on the actual reticle widget to the dyn mat inst
}

void UShooterReticle::OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax)
{
	// Set the material on the actual ammo counter widget to the dyn mat inst
}
