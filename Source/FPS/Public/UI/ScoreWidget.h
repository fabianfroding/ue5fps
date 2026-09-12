// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ScoreWidget.generated.h"

class AShooterPlayerState;
class UTextBlock;

UCLASS()
class FPS_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Score;
	
public:
	virtual void NativeOnInitialized() override;
	
private:
	AShooterPlayerState* GetPlayerState() const;
	
	UFUNCTION()
	void OnScoreChanged(const int32 Score);
	
	UFUNCTION()
	void OnPlayerStateReplicated();
	
};
