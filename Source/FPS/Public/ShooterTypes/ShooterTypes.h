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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ShapeCutFactorRoundFired = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScaleFactorRoundFired = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundFiredInterpSpeed = 20.f;
};
