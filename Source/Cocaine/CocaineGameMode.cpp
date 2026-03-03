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
	Currents.currentMult+= *Mults[MultType];
	SLOG(5.f,FColor::Red,FString::Printf(TEXT("Current Mult: %i"),Currents.currentMult));
	UpdateScore(BaseScoreGain);
	RestartMultFade();
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

void ACocaineGameMode::OnScoreInterval()
{
	SLOG(5.f,FColor::Green,FString::Printf(TEXT("ScoreInterval")));
	UpdateScore(bScoreGoseDown?PassiveScoreGain:-PassiveScoreGain);
}

void ACocaineGameMode::RestartMultFade()
{
	FTimerManager* TimerManager{(&GetWorld()->GetTimerManager())};
	TimerManager->ClearTimer(MultFade);
	TimerManager->SetTimer(MultFade,this,&ACocaineGameMode::OnMultFade,MultFadeDuration);
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
