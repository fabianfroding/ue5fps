// Copyright Fabian Fröding.

#include "UI/ShooterReticle.h"

#include "Characters/ShooterCharacter.h"
#include "Combat/CombatComponent.h"
#include "Components/Image.h"
#include "Weapon/Weapon.h"

namespace Ammo
{
	const FName Rounds_Current = FName("Rounds_Current");
	const FName Rounds_Max = FName("Rounds_Max");
}

namespace Reticle
{
	const FName RoundedCornerScale = FName("RoundedCornerScale");
	const FName ShapeCutThickness = FName("ShapeCutThickness");
	const FName InnerRGBA = FName("Inner_RGBA");
}

void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ImageReticle->SetRenderOpacity(0.f);
	ImageAmmoCounter->SetRenderOpacity(0.f);
	BaseCornerScaleFactorRoundFired = 0.f;
	BaseShapeCutFactorRoundFired = 0.f;
	BaseCornerScaleFactorAiming = 0.f;
	BaseShapeCutFactorAiming = 0.f;
	bAiming = false;
	bTargetingPlayer = false;
	
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	// Pattern to deal with race-condition between pawn possessed and weapon replicated.
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		if (AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter); IsValid(Weapon))
		{
			UpdateReticleAndAmmoCounter(Weapon);
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
			UpdateReticleAndAmmoCounter(Weapon);
		}
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	BaseCornerScaleFactorRoundFired = FMath::FInterpTo(BaseCornerScaleFactorRoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	BaseShapeCutFactorRoundFired = FMath::FInterpTo(BaseShapeCutFactorRoundFired, 0.f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	
	BaseCornerScaleFactorAiming = FMath::FInterpTo(
		BaseCornerScaleFactorAiming, 
		bAiming ? CurrentReticleParams.ScaleFactorAiming : CurrentReticleParams.ScaleFactorNotAiming, 
		InDeltaTime, 
		CurrentReticleParams.AimingInterpSpeed);
	
	BaseShapeCutFactorAiming = FMath::FInterpTo(
		BaseShapeCutFactorAiming,
		bAiming ? CurrentReticleParams.ShapeCutFactorAiming : CurrentReticleParams.ShapeCutFactorNotAiming,
		InDeltaTime,
		CurrentReticleParams.AimingInterpSpeed);
	
	BaseCornerScaleFactorTargetingPlayer = FMath::FInterpTo(
		BaseCornerScaleFactorTargetingPlayer, 
		bTargetingPlayer ? CurrentReticleParams.ScaleFactorTargeting : CurrentReticleParams.ScaleFactorNotTargeting,
		InDeltaTime,
		CurrentReticleParams.TargetingPlayerInterpSpeed);
	
	BaseCornerScaleFactor = BaseCornerScaleFactorRoundFired + BaseCornerScaleFactorAiming + BaseCornerScaleFactorTargetingPlayer;
	BaseShapeCutFactor = BaseShapeCutFactorRoundFired + BaseShapeCutFactorAiming;
	
	if (CurrentReticleDynMatInst.IsValid())
	{
		CurrentReticleDynMatInst->SetScalarParameterValue(Reticle::RoundedCornerScale, BaseCornerScaleFactor);
		CurrentReticleDynMatInst->SetScalarParameterValue(Reticle::ShapeCutThickness, BaseShapeCutFactor);
	}
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombatComponent = UCombatComponent::FindCombatComponent(OldPawn);
	UCombatComponent* NewPawnCombatComponent = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(OldPawnCombatComponent))
	{
		OldPawnCombatComponent->OnReticleChanged.RemoveDynamic(this, &ThisClass::OnReticleChanged);
		OldPawnCombatComponent->OnAmmoCounterChanged.RemoveDynamic(this, &ThisClass::OnAmmoCounterChanged);
		OldPawnCombatComponent->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
		OldPawnCombatComponent->OnAimingStatusChanged.RemoveDynamic(this, &ThisClass::OnAimingStatusChanged);
		OldPawnCombatComponent->OnTargetingPlayerStatusChanged.RemoveDynamic(this, &ThisClass::OnTargetingPlayerStatusChanged);
	}
	if (IsValid(NewPawnCombatComponent))
	{
		ImageReticle->SetRenderOpacity(1.f);
		ImageAmmoCounter->SetRenderOpacity(1.f);
		NewPawnCombatComponent->OnReticleChanged.AddDynamic(this, &ThisClass::OnReticleChanged);
		NewPawnCombatComponent->OnAmmoCounterChanged.AddDynamic(this, &ThisClass::OnAmmoCounterChanged);
		NewPawnCombatComponent->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
		NewPawnCombatComponent->OnAimingStatusChanged.AddDynamic(this, &ThisClass::OnAimingStatusChanged);
		NewPawnCombatComponent->OnTargetingPlayerStatusChanged.AddDynamic(this, &ThisClass::OnTargetingPlayerStatusChanged);
	}
}

void UShooterReticle::OnReticleChanged(UMaterialInstanceDynamic* ReticleDynMatInst, const FReticleParams& ReticleParams, bool bCurrentlyTargetingPlayer)
{
	CurrentReticleParams = ReticleParams;
	CurrentReticleDynMatInst = ReticleDynMatInst;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ReticleDynMatInst);
	if (IsValid(ImageReticle))
	{
		ImageReticle->SetBrush(Brush);
	}
	
	OnTargetingPlayerStatusChanged(bCurrentlyTargetingPlayer);
}

void UShooterReticle::OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax)
{
	CurrentAmmoCounterDynMatInst = AmmoCounterDynMatInst;
	CurrentAmmoCounterDynMatInst->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
	CurrentAmmoCounterDynMatInst->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	
	FSlateBrush Brush;
	Brush.SetResourceObject(AmmoCounterDynMatInst);
	if (IsValid(ImageAmmoCounter))
	{
		ImageAmmoCounter->SetBrush(Brush);
	}
}

void UShooterReticle::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax)
{
	if (!CurrentAmmoCounterDynMatInst.IsValid()) return;
	CurrentAmmoCounterDynMatInst->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
	CurrentAmmoCounterDynMatInst->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	BaseCornerScaleFactorRoundFired += CurrentReticleParams.ScaleFactorRoundFired;
	BaseShapeCutFactorRoundFired += CurrentReticleParams.ShapeCutFactorRoundFired;
}

void UShooterReticle::OnTargetingPlayerStatusChanged(bool bIsTargetingPlayer)
{
	bTargetingPlayer = bIsTargetingPlayer;
	if (CurrentReticleDynMatInst.IsValid())
	{
		const FLinearColor ReticleColor = bTargetingPlayer ? FLinearColor::Red : FLinearColor::White;
		CurrentReticleDynMatInst->SetVectorParameterValue(Reticle::InnerRGBA, ReticleColor);
	}
}

void UShooterReticle::UpdateReticleAndAmmoCounter(AWeapon* Weapon)
{
	if (!IsValid(Weapon)) return;
	OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance(), Weapon->ReticleParams);
	OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
}
