// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"

#include "ShooterCharacter.generated.h"

struct FGameplayTag;
class UCombatComponent;

class UCameraComponent;
class UInputAction;
class USpringArmComponent;

UCLASS()
class FPS_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> CombatComponent;
	
	// 1st person view (arms).
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> CycleWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> FireWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> AimWeaponAction;

public:
	AShooterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	
	/* DEV NOTE.
	 * Could argue that the controller should be handling all input for the sake of single responsibility principle.
	 * However, that would mean that we would have to cast to AShooterCharacter every input to perform the relevant logic on the CombatComponent, 
	 * which is not optimal would also add an additional include.
	 * That is why we have these inputs directly in this class instead.
	 */
	
	void InputCycleWeapon();
	void InputReloadWeapon();
	void InputFireWeaponPressed();
	void InputFireWeaponReleased();
	void InputAimPressed();
	void InputAimReleased();
	
	/* Player Interface */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) override;
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const override { return Mesh1P; }
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const override { return GetMesh(); }
	/* ~PlayerInterface */
	
protected:
	virtual void BeginPlay() override;

};
