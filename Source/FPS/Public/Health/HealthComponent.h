// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedSignature, UHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead,
	DeathStarted,
	DeathFinished
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing=OnRep_DeathState)
	EDeathState DeathState;
	
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_Health, Category = "FPS|Health")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_MaxHealth, Category = "FPS|Health")
	float MaxHealth;
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnMaxHealthChanged;
	
public:
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category = "FPS|Health")
	static UHealthComponent* FindHealthComponent(const AActor* Actor) { return IsValid(Actor) ? Actor->FindComponentByClass<UHealthComponent>() : nullptr; }
	
	UFUNCTION(BlueprintPure)
	float GetHealthNormalized() const;
	
	// Return true if lethal.
	bool ChangeHealthByAmount(float Amount, AActor* Instigator);
	void ChangeMaxHealthByAmount(float Amount, AActor* Instigator);

protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnRep_DeathState(EDeathState OldDeathState);
	
	UFUNCTION()
	void OnRep_Health(float OldHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(float OldMaxHealth);
	
};
