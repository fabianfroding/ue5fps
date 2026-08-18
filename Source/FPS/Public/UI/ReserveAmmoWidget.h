// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ReserveAmmoWidget.generated.h"

class UImage;
class UTextBlock;
class AWeapon;

UCLASS()
class FPS_API UReserveAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ImageWeaponIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextAmmo;
	
public:
	virtual void NativeOnInitialized() override;
	
private:
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon, UMaterialInterface* WeaponIconMaterial);
	
	UFUNCTION()
	void OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve);
	
	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon);
	
};
