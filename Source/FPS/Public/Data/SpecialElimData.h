// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShooterTypes/ShooterTypes.h"

#include "SpecialElimData.generated.h"

USTRUCT(BlueprintType)
struct FSpecialElimInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	ESpecialElimType ElimType = ESpecialElimType::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ElimMessage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ElimIcon = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	int32 SequentialElimCount = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 StreakCount = 0;
	
};

UCLASS()
class FPS_API USpecialElimData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|SpecialElims")
	TMap<ESpecialElimType, FSpecialElimInfo> SpecialElimInfo;
	
};
