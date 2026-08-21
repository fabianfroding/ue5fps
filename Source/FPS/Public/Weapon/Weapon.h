// Copyright Fabian Fröding.

#pragma once

#include <GameplayTagContainer.h>

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>

#include "ShooterTypes/ShooterTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EFireType : uint8
{
	Auto		UMETA(DisplayName = "Automatic"),
	SemiAuto	UMETA(DisplayName = "SemiAutomatic")
};

UENUM(BlueprintType)
enum class EWeaponStatus : uint8
{
	Idle,			// Weapon doing nothing, can fire/reload/cycle
	Firing,			// Currently firing, can't reload/cycle
	Reloading,		// Currently reloading, can't fire/cycle
	Cycling,		// Currently cycling the weapon, can't fire/reload/cycle
	Unequipped		// On our character, but can't do anything
};

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|Fire")
	EFireType FireType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|Fire")
	float FireTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|Ammo")
	int32 MagCapacity;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|Ammo")
	int32 Ammo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|Ammo")
	int32 StartingCarriedAmmo;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Reticle")
	FReticleParams ReticleParams;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Icon")
	TObjectPtr<UMaterialInterface> WeaponIcon;
	
	EWeaponStatus WeaponStatus;
	
protected:
	// Weapon mesh first-person view.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	// Weapon mesh third-person view.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|WeaponType")
	FGameplayTag WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Aiming")
	float AimFOV;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Trace")
	float TraceRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Trace")
	bool bShowDebugTrace = false;
	
private:
	int32 AmmoSequence; // Used for client-side prediction.
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TObjectPtr<UMaterialInterface> ReticleMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TObjectPtr<UMaterialInterface> AmmoCounterMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ReticleDynMatInst;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> AmmoCounterDynMatInst;

public:
	AWeapon();
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	FGameplayTag GetWeaponType() const { return WeaponType; }
	UMaterialInstanceDynamic* GetReticleDynamicMaterialInstance();
	UMaterialInstanceDynamic* GetAmmoCounterDynamicMaterialInstance();
	
	void AttachToOwningPawn(APawn* Pawn) const;
	void WeaponTrace(FHitResult& OutHit, const float TraceLength);
	
	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, const TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, const bool bIsFirstPerson);
	void Auth_Fire();
	void Rep_Fire(const int32 AuthAmmo);
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, const EPhysicalSurface ImpactSurfaceType, const bool bIsFirstPerson);
	
private:
	void SetMeshVisibilities(APawn* OwningPawn) const;
	
};
