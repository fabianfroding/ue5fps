// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ShooterReticle.generated.h"

class AWeapon;
class UImage;

UCLASS()
class FPS_API UShooterReticle : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImageReticle;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImageAmmoCounter;
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentReticleDynMatInst;
	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentAmmoCounterDynMatInst;
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon) { UpdateReticleAndAmmoCounter(Weapon); }
	
	UFUNCTION()
	void OnReticleChanged(UMaterialInstanceDynamic* ReticleDynMatInst);
	
	UFUNCTION()
	void OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax);
	
	UFUNCTION()
	void OnRoundFired(int32 RoundsCurrent, int32 RoundsMax);
	
	void UpdateReticleAndAmmoCounter(AWeapon* Weapon);
	
};
