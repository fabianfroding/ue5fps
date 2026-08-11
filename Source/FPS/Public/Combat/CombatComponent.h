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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData")
	TObjectPtr<UWeaponData> WeaponData;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;
	
protected:
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	AWeapon* CurrentWeapon;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;
	
	UPROPERTY(Transient, Replicated) // Transient - can not save to disk.
	TArray<AWeapon*> WeaponInventory;

public:
	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;
	
	// Cycle to the next weapon in the weapon inventory.
	void InitiateCycleWeapon();
	void InitiateFireWeaponPressed();
	void InitiateFireWeaponReleased();
	void InitiateReloadWeapon();
	void InitiateAimPressed();
	void InitiateAimReleased();
	
	void SpawnWeaponInventory();
	void DestroyWeaponInventory();
	void EquipWeapon(AWeapon* WeaponToEquip);
	
private:
	AWeapon* SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const;
	
	// DEV NOTE: OnRep with param matching the replicated type allows access to the value before the change.
	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);
	
	// Reliable: Guaranteed to reach server, even if packets get lost, there's a handshake, i.e. it will be sent again.
	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool bPressed);
	
	void Local_Aim(const bool bPressed);
	
};
