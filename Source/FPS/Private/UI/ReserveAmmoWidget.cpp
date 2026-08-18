// Copyright Fabian Fröding.

#include "UI/ReserveAmmoWidget.h"

#include "Characters/ShooterCharacter.h"
#include "Combat/CombatComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Weapon/Weapon.h"

void UReserveAmmoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ImageWeaponIcon->SetRenderOpacity(0.f);
	TextAmmo->SetRenderOpacity(0.f);
	
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		if (const AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter); IsValid(Weapon))
		{
			OnCurrentReserveAmmoChanged(IPlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}
	
	if (ShooterCharacter->HasAuthority())
	{
		if (AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter); IsValid(Weapon))
		{
			OnCurrentReserveAmmoChanged(IPlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
		}
	}
}

void UReserveAmmoWidget::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombatComponent = UCombatComponent::FindCombatComponent(OldPawn);
	UCombatComponent* NewPawnCombatComponent = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(OldPawnCombatComponent))
	{
		OldPawnCombatComponent->OnCurrentReserveAmmoChanged.RemoveDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		OldPawnCombatComponent->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
	}
	if (IsValid(NewPawnCombatComponent))
	{
		ImageWeaponIcon->SetRenderOpacity(1.f);
		TextAmmo->SetRenderOpacity(1.f);
		NewPawnCombatComponent->OnCurrentReserveAmmoChanged.AddDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		NewPawnCombatComponent->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
	}
}

void UReserveAmmoWidget::OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon)
{
	// TODO: Change weapon icon.
	
	// RoundsInWeapon / RoundsInReserve
	if (IsValid(TextAmmo))
	{
		// DEV NOTE: NSLOCTEXT - Automatic localization for texts.
		const FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}}"), RoundsInWeapon, RoundsInReserve);
		TextAmmo->SetText(AmmoText);
	}
}

void UReserveAmmoWidget::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve)
{
	if (IsValid(TextAmmo))
	{
		const FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}"), RoundsCurrent, RoundsInReserve);
		TextAmmo->SetText(AmmoText);
	}
}

void UReserveAmmoWidget::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	OnCurrentReserveAmmoChanged(IPlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
}
