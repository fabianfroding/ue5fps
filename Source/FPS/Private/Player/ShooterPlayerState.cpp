// Copyright Fabian Fröding.

#include "Player/ShooterPlayerState.h"


AShooterPlayerState::AShooterPlayerState()
{
	NetUpdateFrequency = 100.f;
	
	ScoredElims = 0;
	Defeats = 0;
	Hits = 0;
	Misses = 0;
	bOnStreak = false;
	HeadShotElims = 0;
	HighestStreak = 0;
	RevengeElims = 0;
	DethroneElims = 0;
	ShowStopperElims = 0;
	bFirstBlood = false;
	bWinner = false;
}

void AShooterPlayerState::AddScoredElim()
{
	++ScoredElims;
}

void AShooterPlayerState::AddDefeat()
{
	++Defeats;
}

void AShooterPlayerState::AddHit()
{
	++Hits;
}

void AShooterPlayerState::AddMiss()
{
	++Misses;
}

void AShooterPlayerState::AddHeadShotElim()
{
	++HeadShotElims;
}

void AShooterPlayerState::AddSequentialElim(int32 SequenceCount)
{
	if (SequentialElims.Contains(SequenceCount))
	{
		SequentialElims[SequenceCount]++;
	}
	else
	{
		SequentialElims.Add(SequenceCount, 1);
	}
	
	// Logic to prevent a higher sequential elim to count as a lower sequential elim (triple also counting as double etc).
	for (auto& Elem : SequentialElims)
	{
		if (Elem.Key < SequenceCount && Elem.Value > 0)
		{
			Elem.Value--;
		}
	}
}

void AShooterPlayerState::UpdateHighestStreak(int32 StreakCount)
{
	if (StreakCount > HighestStreak)
	{
		HighestStreak = StreakCount;
	}
}

void AShooterPlayerState::AddRevengeElim()
{
	++RevengeElims;
}

void AShooterPlayerState::AddDethroneElim()
{
	++DethroneElims;
}

void AShooterPlayerState::AddShowStopperElim()
{
	++ShowStopperElims;
}

void AShooterPlayerState::GetFirstBlood()
{
	bFirstBlood = true;
}

void AShooterPlayerState::IsNowWinner()
{
	bWinner = true;
}
