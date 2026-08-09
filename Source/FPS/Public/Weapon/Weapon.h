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
	UPROPERTY(EditAnywhere, Category = "FPS|WeaponType")
	FGameplayTag WeaponType;

public:
	AWeapon();
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

protected:
	virtual void BeginPlay() override;
	
};
