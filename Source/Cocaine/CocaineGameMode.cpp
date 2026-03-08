// Copyright Epic Games, Inc. All Rights Reserved.

#include "CocaineGameMode.h"

#include "MultSystemUI.h"
#include "Blueprint/UserWidget.h"

#if 0
#define STLOG(duration,color,text) GEngine->AddOnScreenDebugMessage(-1, duration, color, text)
#else
#define STLOG(duration,color,text)
#endif
ACocaineGameMode::ACocaineGameMode()
{
	// stub
}

void ACocaineGameMode::AddMult(const EMultType MultType)
{
	Currents.storedMult+= *Mults[MultType];
	UIWidget->BP_UpdateLastAction(ReturnMultString(MultType));
	UIWidget->BP_UpdateStoredMult(FMath::GetRangePct(0.f,static_cast<float>(MultThreshold),Currents.storedMult));
	if (Currents.storedMult>=MultThreshold)
	{
		Currents.currentMult+=1;
		UIWidget->BP_UpdateMult(Currents.currentMult);
		Currents.storedMult=0;
		UIWidget->BP_UpdateStoredMult(FMath::GetRangePct(0.f,static_cast<float>(MultThreshold),Currents.storedMult));
	}
	STLOG(5.f,FColor::Red,FString::Printf(TEXT("Current Mult: %i"),Currents.currentMult));
	UpdateScore(BaseScoreGain);
	//RestartMultFade();
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
	STLOG(15.f,FColor::Red,FString::Printf(TEXT("Current Score: %i"),Currents.currentScore));
	UIWidget->BP_UpdateScore(Currents.currentScore);
}
/*
void ACocaineGameMode::OnMultFade()
{
	STLOG(5.f,FColor::Red,FString::Printf(TEXT("MultFade")));
	Currents.currentMult=StartingMult;
	UIWidget->BP_UpdateMult(StartingMult);
	RestartMultFade();
}
*/
void ACocaineGameMode::OnStoredMultFade()
{
	Currents.storedMult-=Currents.storedMult>0?1:0;
	UIWidget->BP_UpdateStoredMult(FMath::GetRangePct(0.f,static_cast<float>(MultThreshold),Currents.storedMult));
	if (Currents.storedMult<=0)
	{
		Currents.currentMult-=Currents.currentMult>0?1:0;
		Currents.storedMult=MultThreshold;
		UIWidget->BP_UpdateMult(Currents.currentMult);
		UIWidget->BP_UpdateStoredMult(FMath::GetRangePct(0.f,static_cast<float>(MultThreshold),Currents.storedMult));
	}
	RestartStoredMultFade();
}

void ACocaineGameMode::OnScoreInterval()
{
	STLOG(5.f,FColor::Green,FString::Printf(TEXT("ScoreInterval")));
	UpdateScore(bScoreGoseDown?-PassiveScoreGain:PassiveScoreGain);
}
/*
void ACocaineGameMode::RestartMultFade()
{
	FTimerManager* TimerManager{(&GetWorld()->GetTimerManager())};
	TimerManager->ClearTimer(MultFade);
	TimerManager->SetTimer(MultFade,this,&ACocaineGameMode::OnMultFade,MultFadeDuration);
}
*/
void ACocaineGameMode::RestartStoredMultFade()
{
	FTimerManager* TimerManager{(&GetWorld()->GetTimerManager())};
	TimerManager->ClearTimer(StoredMultFade);
	TimerManager->SetTimer(StoredMultFade,this,&ACocaineGameMode::OnStoredMultFade,StoredMultFadeDuration);
	
}
/*
void ACocaineGameMode::StopMultFade()
{
	GetWorld()->GetTimerManager().ClearTimer(MultFade);
}
*/
void ACocaineGameMode::BeginPlay()
{
	Super::BeginPlay();
	//GetWorld()->GetTimerManager().SetTimer(MultFade,this,&ACocaineGameMode::OnMultFade,MultFadeDuration);
	GetWorld()->GetTimerManager().SetTimer(StoredMultFade,this,&ACocaineGameMode::OnStoredMultFade,StoredMultFadeDuration);
	if (bPassiveScoring) GetWorld()->GetTimerManager().SetTimer(ScoreInterval,this,&ACocaineGameMode::OnScoreInterval,PassiveScoreInterval,true);
	UIWidget = CreateWidget<UMultSystemUI>(GetWorld()->GetFirstPlayerController(),UIWidgetClass);
	UIWidget->AddToViewport();
	
	MultQueue.
}
