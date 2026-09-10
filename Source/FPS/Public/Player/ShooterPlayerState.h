// Copyright Fabian Fröding.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterTypes/ShooterTypes.h"
#include "ShooterPlayerState.generated.h"

UCLASS()
class FPS_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AShooterPlayerState();
	
	void AddScoredElim();
	void AddDefeat();
	void AddHit();
	void AddMiss();
	void AddHeadShotElim();
	void AddSequentialElim(int32 SequenceCount);
	void UpdateHighestStreak(int32 StreakCount);
	void AddRevengeElim();
	void AddDethroneElim();
	void AddShowStopperElim();
	void GotFirstBlood();
	void IsNowWinner();
	void SetOnStreak(bool bIsOnStreak);
	bool IsOnStreak() const;
	
	void SetLastAttacker(APlayerState* Attacker);
	APlayerState* GetLastAttacker() const;
	int32 GetScoredElims() const;
	
	UFUNCTION(Client, Reliable)
	void Client_LostTheLead();
	
	UFUNCTION(Client, Reliable)
	void Client_ScoredElim(int32 ElimScore);
	
	UFUNCTION(Client, Reliable)
	void Client_SpecialElim(const ESpecialElimType& SpecialElim, int32 SequentialElimCount, int32 StreakCount, int32 ElimScore);
	
private:
	int32 ScoredElims;
	int32 Defeats;
	int32 Hits;
	int32 Misses;
	bool bOnStreak; // Number of eliminations since we spawned.
	int32 HeadShotElims;
	TMap<int, int32> SequentialElims; // Multiple eliminations within short period of time (double/tripe kills etc).
	int32 HighestStreak;
	int32 RevengeElims;
	int32 DethroneElims;
	int32 ShowStopperElims; // "Shutdowns".
	bool bFirstBlood;
	bool bWinner;
	
	TWeakObjectPtr<APlayerState> LastAttacker;
	
};
