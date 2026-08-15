#pragma once

#include "ShooterTypes.generated.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	Left		UMETA(DisplayName = "TurningLeft"),
	Right		UMETA(DisplayName = "TurningRight"),
	NotTurning	UMETA(DisplayName = "NotTurning"),
};

USTRUCT(BlueprintType)
struct FReticleParams
{
	GENERATED_BODY()
	
	// Shape Cut Factor
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShapeCutFactorRoundFired = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShapeCutFactorAiming = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShapeCutFactorNotAiming = 0.f;
	
	// Scale Factor
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScaleFactorRoundFired = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScaleFactorAiming = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScaleFactorNotAiming = 0.f;
	
	// Interp Speeds
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundFiredInterpSpeed = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimingInterpSpeed = 15.f;
};
