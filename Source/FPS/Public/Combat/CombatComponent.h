// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>

#include "CombatComponent.generated.h"

class AWeapon;
class UWeaponData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "FPS|WeaponData")
	TObjectPtr<UWeaponData> WeaponData;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TSubclassOf<AWeapon> DefaultWeaponClass;

public:
	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Cycle to the next weapon in the inventory.
	void InitiateCycleWeapon();
	void InitiateFireWeaponPressed();
	void InitiateFireWeaponReleased();
	void InitiateReloadWeapon();
	void InitiateAimPressed();
	void InitiateAimReleased();
	
	void SpawnInventory();
	void DestroyInventory();
	
private:
	AWeapon* SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const;
	
};
