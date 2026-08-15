// Copyright Fabian Fröding.

#pragma once

#include <GameplayTagContainer.h>

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EFireType : uint8
{
	Auto		UMETA(DisplayName = "Automatic"),
	SemiAuto	UMETA(DisplayName = "SemiAutomatic")
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
	virtual void OnRep_Instigator() override;
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	FGameplayTag GetWeaponType() const { return WeaponType; }
	UMaterialInstanceDynamic* GetReticleDynamicMaterialInstance();
	UMaterialInstanceDynamic* GetAmmoCounterDynamicMaterialInstance();
	
	void AttachToOwningPawn() const;
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
