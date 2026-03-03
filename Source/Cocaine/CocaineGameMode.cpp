// Copyright Epic Games, Inc. All Rights Reserved.

#include "CocaineGameMode.h"

ACocaineGameMode::ACocaineGameMode()
{
	// stub
}

void ACocaineGameMode::AddMult(const EMultType MultType)
{
	Currents.currentMult+= *Mults[MultType];
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Current Mult: %f"),Currents.currentMult));
	UpdateScore(10.f);
	RestartMultFade();
}

void ACocaineGameMode::CreateMults()
{
	Mults.Add(EMultType::Slide,&SlideMultValue);
	Mults.Add(EMultType::Dash,&DashMultValue);
	Mults.Add(EMultType::Kick,&KickMultValue);
	Mults.Add(EMultType::Jump,&JumpMultValue);
	Mults.Add(EMultType::Grind,&GrindMultValue);
	Mults.Add(EMultType::Mantle,&MantleMultValue);
}

void ACocaineGameMode::UpdateScore(const float& Score)
{
	Currents.currentScore+=Score*Currents.currentMult;
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Current Score: %f"),Currents.currentScore));
}

void ACocaineGameMode::OnMultFade()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Cleared Mult")));
	Currents.currentMult=1.f;
	RestartMultFade();
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
}

void ACocaineGameMode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	CreateMults();
}
