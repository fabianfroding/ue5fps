// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>

#include "GameplayTagContainer.h"
#include "CombatComponent.generated.h"

class AWeapon;
class UWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams, bool, bCurrentlyTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRoundFired, int32, RoundsCurrent, int32, RoundsMax, int32, RoundsInReserve);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChanged, bool, bIsTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCurrentReserveAmmoChanged, int32, RoundsInReserve, int32, RoundsInWeapon, UMaterialInterface*, WeaponIconMaterial);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData")
	TObjectPtr<UWeaponData> WeaponData;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentReserveAmmo)
	int32 CurrentReserveAmmo;
	
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
	
	UPROPERTY(BlueprintAssignable)
	FCurrentReserveAmmoChanged OnCurrentReserveAmmoChanged;
	
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
	
	TMap<FGameplayTag, int32> ReserveAmmo;
	
	bool bTriggerPressed;
	FTimerHandle FireTimer;
	bool bHitPlayerLastFrame;
	bool bHitPlayer;
	
	int32 LocalWeaponIndex;

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
	bool IsTargetingPlayer() const { return bHitPlayer; }
	
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
	
	UFUNCTION()
	void OnRep_CurrentReserveAmmo();
	
	void Local_FireWeapon();
	
	void FireTimerFinished();
	
	void TraceForPlayer();
	
	int32 AdvanceWeaponIndex();
	
};
