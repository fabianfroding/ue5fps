// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>

#include "CombatComponent.generated.h"

class AWeapon;
class UWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoundFired, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChanged, bool, bIsTargetingPlayer);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData")
	TObjectPtr<UWeaponData> WeaponData;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;
	
	UPROPERTY(BlueprintAssignable)
	FReticleChanged OnReticleChanged;
	
	UPROPERTY(BlueprintAssignable)
	FAmmoCounterChanged OnAmmoCounterChanged;
	
	UPROPERTY(BlueprintAssignable)
	FRoundFired OnRoundFired;
	
	UPROPERTY(BlueprintAssignable)
	FAimingStatusChanged OnAimingStatusChanged;
	
	UPROPERTY(BlueprintAssignable)
	FTargetingPlayerStatusChanged OnTargetingPlayerStatusChanged;
	
protected:
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	AWeapon* CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	float TraceLength;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;
	
	UPROPERTY(Transient, Replicated) // Transient - can not save to disk.
	TArray<AWeapon*> WeaponInventory;
	
	bool bTriggerPressed;
	FTimerHandle FireTimer;
	bool bHitPlayerLastFrame;

public:
	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;
	
	UFUNCTION(BlueprintPure, Category = "FPS|Combat")
	static UCombatComponent* FindCombatComponent(const AActor* Actor) { return IsValid(Actor) ? Actor->FindComponentByClass<UCombatComponent>() : nullptr; }
	
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
	void InitializeWeaponWidgets() const;
	
	AWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	
private:
	AWeapon* SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const;
	
	// DEV NOTE: OnRep with param matching the replicated type allows access to the value before the change.
	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);
	
	// Reliable: Guaranteed to reach server, even if packets get lost, there's a handshake, i.e. it will be sent again.
	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool bPressed);
	
	void Local_Aim(const bool bPressed);
	
	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FHitResult& Hit, const int32 AuthAmmo);
	
	void Local_FireWeapon();
	
	void FireTimerFinished();
	
	void TraceForPlayer();
	
};
