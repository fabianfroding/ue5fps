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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScaleFactorTargeting = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScaleFactorNotTargeting = 0.f;
	
	// Interp Speeds
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundFiredInterpSpeed = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimingInterpSpeed = 15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TargetingPlayerInterpSpeed = 10.f;
};

UENUM(meta = (BitFlags))
enum class ESpecialElimType : uint16
{
	None			= 0,
	HeadShot		= 1 << 0,		// 00000000 00000001
	Sequential		= 1 << 1,		// 00000000 00000010
	Streak			= 1 << 2,		// 00000000 00000100
	Revenge			= 1 << 3,		// 00000000 00001000
	Dethrone		= 1 << 4,		// 00000000 00010000
	Showstopper		= 1 << 5,		// 00000000 00100000
	FirstBlood		= 1 << 6,		// 00000000 01000000
	GainedTheLead	= 1 << 7,		// 00000000 10000000
	TiedTheLeader	= 1 << 8,		// 00000001 00000000
	LostTheLead		= 1 << 9		// 00000010 00000000
};

ENUM_CLASS_FLAGS(ESpecialElimType)
