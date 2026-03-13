// Copyright Epic Games, Inc. All Rights Reserved.

#include "CocaineGameMode.h"

#include "MultSystemUI.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#if 0
#define STLOG(duration,color,text) GEngine->AddOnScreenDebugMessage(-1, duration, color, text)
#else
#define STLOG(duration,color,text)
#endif
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
	if (Currents.currentMult<=1)
	{
		RestartStoredMultFade();
		return;
	}
	if (Currents.storedMult<=0)
	{
		Currents.currentMult-=Currents.currentMult>1?1:0;
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

void ACocaineGameMode::OnSlowMoDrain()
{
	Currents.storedMult-=SlowMoCost;
	if (Currents.storedMult<=0) StopSlowMo();
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

}

void ACocaineGameMode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

ACocaineGameMode::ACocaineGameMode()
{
	// stub
}

void ACocaineGameMode::AddMult(const EMultType MultType)
{
	if (!CanAddMult(MultType)) return;
	Currents.storedMult+= *Mults[MultType];
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

void ACocaineGameMode::StartSlowMo()
{
	if (Currents.storedMult<=0) return;
	const UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(SlowMoDrainTimer,this,&ACocaineGameMode::OnSlowMoDrain,SlowMoDrainInterval,true);
	UGameplayStatics::SetGlobalTimeDilation(World,TimeScale);
}

void ACocaineGameMode::StopSlowMo()
{
	const UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(SlowMoDrainTimer);
	UGameplayStatics::SetGlobalTimeDilation(World,1);
}

void ACocaineGameMode::AddToHistory(const EMultType& MultType)
{
	MultHistory.Emplace(MultType);
	MultHistory.RemoveAt(0);
	AddToDisplayedHistory();
}
void ACocaineGameMode::AddToDisplayedHistory()
{
	FString ReturnString{};
	for (const EMultType Historic : MultHistory)
	{
		if (Historic==Empty) continue;
		ReturnString+=ReturnMultString(Historic)+"\n";
	}
	UIWidget->BP_UpdateLastAction(ReturnString);
}

bool ACocaineGameMode::CanAddMult(const EMultType MultType)
{
	for (const EMultType Historic : MultHistory)
	{
		if (ReturnMultString(Historic)==ReturnMultString(MultType))
		{
			AddToHistory(MultType);
			return false;
		}
	}
	AddToHistory(MultType);
		return true;
}
