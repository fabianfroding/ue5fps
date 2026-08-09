// Copyright Fabian Fröding.

#include "Characters/ShooterCharacter.h"

#include <EnhancedInputComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

#include "Camera/CameraComponent.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/SpringArmComponent.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->MovementState.bCanCrouch = true;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->bUsePawnControlRotation = true;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(SpringArm);
	FirstPersonCamera->bUsePawnControlRotation = false;
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->SetupAttachment(FirstPersonCamera);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false; // Optimization.
	Mesh1P->bReceivesDecals = false; // Optimization. Used for bullet holes etc, irrelevant for this mesh.
	// Optimization. Only animate for the player who can see the mesh.
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	// Optimization. Allow the mesh to update location, transform etc. before ticking physics. Allows more smooth camera follow. Avoids jitters/stutters.
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	ShooterInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::InputCycleWeapon);
	ShooterInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::InputReloadWeapon);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::InputFireWeaponPressed);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AShooterCharacter::InputFireWeaponReleased);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::InputAimPressed);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &AShooterCharacter::InputAimReleased);
}

void AShooterCharacter::InputCycleWeapon()
{
	CombatComponent->InitiateCycleWeapon();
}

void AShooterCharacter::InputReloadWeapon()
{
	CombatComponent->InitiateReloadWeapon();
}

void AShooterCharacter::InputFireWeaponPressed()
{
	CombatComponent->InitiateFireWeaponPressed();
}

void AShooterCharacter::InputFireWeaponReleased()
{
	CombatComponent->InitiateFireWeaponReleased();
}

void AShooterCharacter::InputAimPressed()
{
	CombatComponent->InitiateAimPressed();
}

void AShooterCharacter::InputAimReleased()
{
	CombatComponent->InitiateAimReleased();
}
