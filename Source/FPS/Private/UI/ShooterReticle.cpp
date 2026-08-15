// Copyright Fabian Fröding.

#include "UI/ShooterReticle.h"

#include "Characters/ShooterCharacter.h"


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
			// Get dynamic material instances from the weapon.
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
	// Unbind delegates from old pawn's combat component.
	// Bind delegates to new pawn's combat component.
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	// Get dynamic material instances from the weapon.
}
