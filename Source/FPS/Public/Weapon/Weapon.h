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
	
private:
	// Weapon mesh first-person view.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	// Weapon mesh third-person view.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|WeaponType")
	FGameplayTag WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Aiming")
	float AimFOV;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Trace")
	float TraceRadius;

public:
	AWeapon();
	virtual void OnRep_Instigator() override;
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	FGameplayTag GetWeaponType() const { return WeaponType; }
	
	void AttachToOwningPawn() const;
	void WeaponTrace(FHitResult& OutHit, const float TraceLength);

protected:
	virtual void BeginPlay() override;
	
private:
	void SetMeshVisibilities(APawn* OwningPawn) const;
	
};
