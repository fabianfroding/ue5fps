// Copyright Fabian Fröding.

#pragma once

#include <GameplayTagContainer.h>

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>

#include "Weapon.generated.h"

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
protected:
	// Weapon mesh first-person view.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	// Weapon mesh third-person view.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|WeaponType")
	FGameplayTag WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Aiming")
	float AimFOV;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Trace")
	float TraceRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Trace")
	bool bShowDebugTrace = false;

public:
	AWeapon();
	virtual void OnRep_Instigator() override;
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	FGameplayTag GetWeaponType() const { return WeaponType; }
	
	void AttachToOwningPawn() const;
	void WeaponTrace(FHitResult& OutHit, const float TraceLength);
	
	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, const TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, const bool bIsFirstPerson);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, const EPhysicalSurface ImpactSurfaceType, const bool bIsFirstPerson);
	
private:
	void SetMeshVisibilities(APawn* OwningPawn) const;
	
};
