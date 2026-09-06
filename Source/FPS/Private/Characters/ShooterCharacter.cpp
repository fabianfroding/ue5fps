// Copyright Fabian Fröding.

#include "Characters/ShooterCharacter.h"

#include <EnhancedInputComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

#include "Camera/CameraComponent.h"
#include "Combat/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/WeaponData.h"
#include "Elimination/EliminationComponent.h"
#include "FPS/FPS.h"
#include "Game/ShooterGameModeBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Health/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ShooterPlayerController.h"
#include "Weapon/Weapon.h"

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
	
	EliminationComponent = CreateDefaultSubobject<UEliminationComponent>("EliminationComponent");
	EliminationComponent->SetIsReplicated(false); // Handled server-side.
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->SetIsReplicated(true);
	
	DefaultFOV = 90.f;
	TurningStatus = ETurningInPlace::NotTurning;
	
	bWeaponFirstReplicated = false;
	RespawnTime = 5.f;
	
	// DEV NOTE: Calling virtual functions in constructors is bad practice. Issues usually arise when using subclasses.
}

void AShooterCharacter::WeaponReplicated_Implementation()
{
	if (!bWeaponFirstReplicated)
	{
		bWeaponFirstReplicated = true;
		OnWeaponFirstReplicated.Broadcast(CombatComponent->GetCurrentWeapon());
	}
}

AWeapon* AShooterCharacter::GetCurrentWeapon_Implementation()
{
	return CombatComponent->GetCurrentWeapon();
}

int32 AShooterCharacter::GetReserveAmmo_Implementation() const
{
	return CombatComponent->CurrentReserveAmmo;
}

void AShooterCharacter::NotifyCycleWeapon_Implementation()
{
	CombatComponent->NotifyCycleWeapon();
}

void AShooterCharacter::NotifyReloadWeapon_Implementation()
{
	CombatComponent->NotifyReloadWeapon();
}

void AShooterCharacter::AddAmmo_Implementation(const FGameplayTag& WeaponType, int32 AmmoAmount)
{
	if (HasAuthority() && IsValid(CombatComponent))
	{
		CombatComponent->AddAmmo(WeaponType, AmmoAmount);
	}
}

bool AShooterCharacter::DoDamage_Implementation(float Damage, AActor* DamageInstigator)
{
	if (!IsValid(HealthComponent)) return false;
	
	if (HealthComponent->ChangeHealthByAmount(-Damage, DamageInstigator))
	{
		// ChangeHealthByAmount returns true if lethal. Also, we can skip hit montage if death.
		return true;
	}
	
	const int32 MontageSelection = FMath::RandRange(0, HitReacts.Num() - 1);
	Multicast_HitReact(MontageSelection);
	return false;
}

void AShooterCharacter::Multicast_HitReact_Implementation(int32 MontageIndex)
{
	if (GetNetMode() != NM_DedicatedServer && !IsLocallyControlled())
	{
		if (HitReacts.IsValidIndex(MontageIndex))
		{
			GetMesh()->GetAnimInstance()->Montage_Play(HitReacts[MontageIndex]);
		}
	}
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	
	FirstPersonCamera->SetFieldOfView(DefaultFOV);
	
	StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
	
	if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetController()); IsValid(ShooterPlayerController))
	{
		ShooterPlayerController->bPawnAlive = true;
	}
	
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("AShooterCharacter::BeginPlay: Bind."));
		// We bind this here, because if we try bind it in the elim component we can't guarantee that combat comp is created yet.
		// But from actor that owns the component, we can, since combat was created first in the constructor.
		CombatComponent->OnRoundReported.AddDynamic(EliminationComponent, &UEliminationComponent::OnRoundReported);
	}
}

void AShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	if (IsValid(CombatComponent))
	{
		CombatComponent->DestroyWeaponInventory();
	}
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateTurnInPlaceParameters(DeltaTime);
	CalculateFABRIKSocketTransform();
}

void AShooterCharacter::CalculateFABRIKSocketTransform()
{
	if (IsValid(CombatComponent) && CombatComponent->GetCurrentWeapon() && IsValid(CombatComponent->GetCurrentWeapon()->GetMesh3P()))
	{
		FABRIKSocketTransform = CombatComponent->GetCurrentWeapon()->GetMesh3P()->GetSocketTransform("FABRIK_Socket", RTS_World);
		FVector OutLocation;
		FRotator OutRotation;
		GetMesh()->TransformToBoneSpace("hand_r", 
			FABRIKSocketTransform.GetLocation(), 
			FABRIKSocketTransform.GetRotation().Rotator(), 
			OutLocation, OutRotation);
		FABRIKSocketTransform.SetLocation(OutLocation);
		FABRIKSocketTransform.SetRotation(OutRotation.Quaternion());
	}
}

void AShooterCharacter::CalculateTurnInPlaceParameters(const float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	const float Speed = Velocity.Size();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();
	
	// Standing still and not jumping.
	if (Speed == 0.f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		
		// Get delta aim rotation - the diff in rotation of my current aim rot from the initial aim rot (StartingAimRotation is set in BeginPlay).
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AOYaw = DeltaAimRotation.Yaw;
		
		if (TurningStatus == ETurningInPlace::NotTurning)
		{
			InterpAOYaw = AOYaw;
		}
		
		// Interpolate the InterpAOYaw value to zero.
		TurnInPlace(DeltaTime);
	}
	
	// Running or jumping.
	if (Speed > 0.f || bIsInAir)
	{
		// Reset initial aim rot to the actual current aim rot.
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AOYaw = 0.f;
		FRotator AimRotation = GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());
		
		// Get movement offset yaw to use in strafe blendspaces - delta between our movement rot and aim rot.
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		TurningStatus = ETurningInPlace::NotTurning;
	}
	
	AOYaw *= -1.f;
}

void AShooterCharacter::TurnInPlace(const float DeltaTime)
{
	if (AOYaw > 90.f)
	{
		TurningStatus = ETurningInPlace::Right;
	}
	else if (AOYaw < -90.f)
	{
		TurningStatus = ETurningInPlace::Left;
	}
	
	// We are turning.
	if (TurningStatus != ETurningInPlace::NotTurning)
	{
		// Interpolate InterpAO_Yaw down to zero.
		InterpAOYaw = FMath::FInterpTo(InterpAOYaw, 0.f, DeltaTime, 4.f);
		AOYaw = InterpAOYaw;
		
		if (FMath::Abs(AOYaw) < 5.f)
		{
			TurningStatus = ETurningInPlace::NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void AShooterCharacter::OnDeathStarted()
{
	if (HasAuthority())
	{
		CombatComponent->DestroyWeaponInventory();
		GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AShooterCharacter::DeathTimerFinished, RespawnTime);
	}
	if (GetNetMode() != NM_DedicatedServer)
	{
		DeathEffects();
		if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetController()); IsValid(ShooterPlayerController))
		{
			DisableInput(ShooterPlayerController);
			if (ShooterPlayerController->IsLocalController())
			{
				ShooterPlayerController->bPawnAlive = false;
			}
		}
	}
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(FPSTraceChannels::ECC_Weapon, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(FPSTraceChannels::ECC_Weapon, ECR_Ignore);
}

void AShooterCharacter::DeathTimerFinished()
{
	if (AShooterGameModeBase* ShooterGameMode = Cast<AShooterGameModeBase>(UGameplayStatics::GetGameMode(this)); IsValid(ShooterGameMode))
	{
		ShooterGameMode->RequestRespawn(this, GetController());
	}
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

void AShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (IsValid(CombatComponent))
	{
		CombatComponent->SpawnWeaponInventory();
	}
}

void AShooterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (IsValid(CombatComponent))
	{
		CombatComponent->InitializeWeaponWidgets();
	}
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
	OnAim(true);
}

void AShooterCharacter::InputAimReleased()
{
	CombatComponent->InitiateAimReleased();
	OnAim(false);
}

FRotator AShooterCharacter::GetFixedAimRotation() const
{
	// DEV NOTE: GetAimBaseRotation uses a compressed replicated value. We need to map the pitch [270, 360] to [-90, 0].
	// This is to prevent the 3P-mesh from snapping upwards for other players when the local player aims down.
	FRotator AimRotation = GetBaseAimRotation();
	if (AimRotation.Pitch > 90.f && !IsLocallyControlled())
	{
		const FVector2D InRange(270, 360);
		const FVector2D OutRange(-90, 0);
		AimRotation.Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimRotation.Pitch);
	}
	return AimRotation;
}

bool AShooterCharacter::HasCurrentWeapon() const
{
	// Valid check since this is used in Anim BPs and might run before the combat component is valid.
	return IsValid(CombatComponent) && CombatComponent->GetCurrentWeapon() != nullptr;
}

FName AShooterCharacter::GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType)
{
	checkf(CombatComponent->WeaponData, TEXT("AShooterCharacter::GetWeaponAttachPoint_Implementation: No Weapon Data Asset - Please fill out BP_ShooterCharacter"));
	return CombatComponent->WeaponData->GripPoints.FindChecked(WeaponType);
}
