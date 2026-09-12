// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/PlayerController.h>

#include "ShooterPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStateReplicated);

UCLASS()
class FPS_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	bool bPawnAlive;
	
	UPROPERTY(BlueprintAssignable)
	FPlayerStateReplicated OnPlayerStateReplicated;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Input")
	TObjectPtr<UInputMappingContext> ShooterIMC;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Input")
	TObjectPtr<UInputAction> LookInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Input")
	TObjectPtr<UInputAction> MoveInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Input")
	TObjectPtr<UInputAction> JumpInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Input")
	TObjectPtr<UInputAction> CrouchInputAction;
	
public:
	AShooterPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnRep_PlayerState() override;
	
private:
	void InputCrouch();
	void InputJump();
	void InputLook(const FInputActionValue& InputActionValue);
	void InputMove(const FInputActionValue& InputActionValue);
	
};
