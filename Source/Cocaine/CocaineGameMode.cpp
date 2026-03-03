// Copyright Epic Games, Inc. All Rights Reserved.

#include "CocaineGameMode.h"

#if 1
#define SLOG(duration,color,text) GEngine->AddOnScreenDebugMessage(-1, duration, color, text)
#else
#define SLOG(duration,color,text)
#endif
ACocaineGameMode::ACocaineGameMode()
{
	// stub
}

void ACocaineGameMode::AddMult(const EMultType MultType)
{
	Currents.storedMult+= *Mults[MultType];
	if (Currents.storedMult>=MultThreshold)
	{
		Currents.currentMult*=2;
		Currents.storedMult=0;
	}
	SLOG(5.f,FColor::Red,FString::Printf(TEXT("Current Mult: %i"),Currents.currentMult));
	UpdateScore(BaseScoreGain);
	RestartMultFade();
	RestartStoredMultFade();
}

int ACocaineGameMode::GetMultValue() const
{
	return Currents.currentMult;
}

int ACocaineGameMode::GetStoredMultValue() const
{
	return Currents.storedMult;
}

int ACocaineGameMode::GetScore() const
{
	return Currents.currentScore;
}


void ACocaineGameMode::UpdateScore(const float& Score)
{
	Currents.currentScore+=Score*Currents.currentMult;
	SLOG(15.f,FColor::Red,FString::Printf(TEXT("Current Score: %i"),Currents.currentScore));
}

void ACocaineGameMode::OnMultFade()
{
	SLOG(5.f,FColor::Red,FString::Printf(TEXT("MultFade")));
	Currents.currentMult=StartingMult;
	RestartMultFade();
}

void ACocaineGameMode::OnStoredMultFade()
{
	Currents.storedMult-=Currents.storedMult>0?1:0;
}

void ACocaineGameMode::OnScoreInterval()
{
	SLOG(5.f,FColor::Green,FString::Printf(TEXT("ScoreInterval")));
	UpdateScore(bScoreGoseDown?-PassiveScoreGain:PassiveScoreGain);
}

void ACocaineGameMode::RestartMultFade()
{
	FTimerManager* TimerManager{(&GetWorld()->GetTimerManager())};
	TimerManager->ClearTimer(MultFade);
	TimerManager->SetTimer(MultFade,this,&ACocaineGameMode::OnMultFade,MultFadeDuration);
}

void ACocaineGameMode::RestartStoredMultFade()
{
	FTimerManager* TimerManager{(&GetWorld()->GetTimerManager())};
	TimerManager->ClearTimer(StoredMultFade);
	TimerManager->SetTimer(StoredMultFade,this,&ACocaineGameMode::OnStoredMultFade,StoredMultFadeDuration);
	
}

void ACocaineGameMode::StopMultFade()
{
	GetWorld()->GetTimerManager().ClearTimer(MultFade);
}

void ACocaineGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(MultFade,this,&ACocaineGameMode::OnMultFade,MultFadeDuration);
	if (bPassiveScoring) GetWorld()->GetTimerManager().SetTimer(ScoreInterval,this,&ACocaineGameMode::OnScoreInterval,PassiveScoreInterval,true);
}
