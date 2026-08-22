// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "ShooterTypes/ShooterTypes.h"

#include "ShooterCharacter.generated.h"

struct FGameplayTag;
class UCombatComponent;

class UCameraComponent;
class UInputAction;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponFirstReplicated, AWeapon*, Weapon);

UCLASS()
class FPS_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "FPS|FABRIK")
	FTransform FABRIKSocketTransform;
	
	UPROPERTY(BlueprintAssignable)
	FWeaponFirstReplicated OnWeaponFirstReplicated;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Combat")
	TObjectPtr<UCombatComponent> CombatComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|Aiming")
	float DefaultFOV;
	
	UPROPERTY(BlueprintReadOnly, Category = "FPS|TurnInPlace")
	float AOYaw;
	
	UPROPERTY(BlueprintReadOnly, Category = "FPS|Strafing")
	float MovementOffsetYaw;
	
	UPROPERTY(BlueprintReadOnly, Category = "FPS|TurnInPlace")
	ETurningInPlace TurningStatus = ETurningInPlace::NotTurning;
	
private:
	// 1st person view (arms).
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> CycleWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> FireWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> AimWeaponAction;
	
	FRotator StartingAimRotation;
	float InterpAOYaw;
	
	bool bWeaponFirstReplicated;

public:
	AShooterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
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
	
	// DEV NOTE: Functions that need to be accessed in thread-safe function (such as Anim BPs) need to be const.
	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;
	
	UFUNCTION(BlueprintPure)
	bool HasCurrentWeapon() const;
	
	bool HasWeaponFirstReplicated() const { return bWeaponFirstReplicated; }
	
	/* Player Interface */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) override;
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const override { return Mesh1P; }
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const override { return GetMesh(); }
	virtual void WeaponReplicated_Implementation() override;
	virtual AWeapon* GetCurrentWeapon_Implementation() override;
	virtual int32 GetReserveAmmo_Implementation() const override;
	virtual void NotifyCycleWeapon_Implementation() override;
	/* ~PlayerInterface */
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(const bool bIsAiming);
	
private:
	void CalculateFABRIKSocketTransform();
	void CalculateTurnInPlaceParameters(const float DeltaTime);
	void TurnInPlace(const float DeltaTime);

};
