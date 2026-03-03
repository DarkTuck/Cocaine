// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CocaineGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */


UENUM()
enum EMultType
{
	Slide,
	Jump,
	Kick,
	Dash,
	Grind,
	Mantle,
	Kill
};
USTRUCT()
struct FCurrents
{
	GENERATED_BODY()
	int currentScore;
	int currentMult;
};

UCLASS(abstract)
class ACocaineGameMode : public AGameModeBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int SlideMultValue=2;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int JumpMultValue=2;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int KickMultValue=2;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int DashMultValue=2;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int GrindMultValue=2;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int MantleMultValue=2;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int KillMultValue=2;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") float MultFadeDuration=5;
	
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System") int StartingScore=0;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System") int BaseScoreGain=10;
	
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring") int PassiveScoreGain=10;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring") float PassiveScoreInterval=1.f;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring",DisplayName="Should Player gain/losse score passively") bool bPassiveScoring;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring",DisplayName="Should score go down by default") bool bScoreGoseDown;
	
	int SavedScore;
	static constexpr int StartingMult=1;
	FCurrents Currents{StartingScore,StartingMult};
	FTimerHandle MultFade;
	FTimerHandle ScoreInterval;
	EMultType CurrentMultType;
	void UpdateScore(const float& Score);
	void OnMultFade();
	void OnScoreInterval();
	
	void RestartMultFade();
	void StopMultFade();

	
public:
	virtual void BeginPlay() override;
	ACocaineGameMode();
	UFUNCTION(BlueprintCallable)
	void AddMult(const EMultType MultType);
private:
	const TMap<EMultType, int*> Mults{
	{Slide,&SlideMultValue},
		{Jump,&JumpMultValue},
		{Kick,&KickMultValue},
		{Dash,&DashMultValue},
		{Grind,&GrindMultValue},
		{Mantle,&MantleMultValue},
		{Kill,&KillMultValue},
	};	
};



