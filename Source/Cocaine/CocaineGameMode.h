// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MultSystemUI.h"
#include "GameFramework/GameModeBase.h"
#include "CocaineGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 *  
 *  Thank you, Epic Games developers who made First-Person Shooter template
 *  for kindly putting ui scoring logic in GameMode, which made me think It's the appropriate class
 *  for that kind of logic. Well It was until I learned its server's class.
 *  It changes nothing for now as this will be singleplayer game,
 *  but it will need to be changed if we want to add multiplayer mode
 */


UENUM()
enum EMultType
{
	Empty,
	Slide,
	Jump,
	Kick,
	Dash,
	Grind,
	Mantle,
	Kill,
	Grapple
};


USTRUCT()
struct FCurrents
{
	GENERATED_BODY()
	int currentScore;
	int currentMult;
	int storedMult;
	
	// UPROPERTY(EditAnywhere, Category = "Scoring|Mult System") int32 MultHistoryLimit = 3;
};

UCLASS(abstract)
class ACocaineGameMode : public AGameModeBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int SlideMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int JumpMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int KickMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int DashMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int GrindMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int MantleMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int KillMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|Mult Values") int GrappleMultValue=20;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|System Settings") int MultThreshold=100;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|System Settings") float MultFadeDuration=5;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System|System Settings") float StoredMultFadeDuration=2.5f;
	
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System") int StartingScore=0;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System") int BaseScoreGain=10;
	
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring") int PassiveScoreGain=10;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring") float PassiveScoreInterval=1.f;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring",DisplayName="Should Player gain/losse score passively") bool bPassiveScoring;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Score System|Passive Scorring",DisplayName="Should score go down by default") bool bScoreGoseDown;
	
	UPROPERTY(EditDefaultsOnly,Category="Scoring|SlowMo",meta=(ClampMin=0,ClampMax=1)) float TimeScale=.5f;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|SlowMo") int SlowMoCost=10;
	UPROPERTY(EditDefaultsOnly,Category="Scoring|SlowMo") float SlowMoDrainInterval=1.f;
	
	
	UPROPERTY(EditDefaultsOnly,Category="Scoring|UI") TSubclassOf<UMultSystemUI> UIWidgetClass;
	TObjectPtr<UMultSystemUI> UIWidget;
	
	int SavedScore;
	static constexpr int StartingMult=1;
	FCurrents Currents{StartingScore,StartingMult};
	//FTimerHandle MultFade;
	FTimerHandle StoredMultFade;
	FTimerHandle ScoreInterval;
	FTimerHandle SlowMoDrainTimer;
	EMultType CurrentMultType;
	
	UPROPERTY(EditDefaultsOnly,Category="Scoring|Mult System") int MultHistoryLimit=3;
	TArray<EMultType> MultHistory={Empty,Empty,Empty};
	
	static constexpr int MinCurrentMult = 1;
	static constexpr int MinStoredMult = 0;
	bool bIsSlowMo;
	
	void UpdateScore(const float& Score);
	
	//void OnMultFade();
	void OnStoredMultFade();
	void OnScoreInterval();
	void OnSlowMoDrain();
	
	//void RestartMultFade();
	void RestartStoredMultFade();
	//void StopMultFade();

	
public:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	ACocaineGameMode();
	UFUNCTION(BlueprintCallable)
	void AddMult(const EMultType MultType);
	UFUNCTION(BlueprintCallable)
	int GetMultValue() const;
	UFUNCTION(BlueprintCallable)
	int GetStoredMultValue() const;
	UFUNCTION(BlueprintCallable)
	int GetScore() const;
	
	void StartSlowMo();
	void StopSlowMo();

private:
	
	void AddToHistory(const EMultType& MultType);
	void AddToDisplayedHistory();
	
	void DrainMult();
	bool CanDrainMult() const;
	
	bool CanAddMult(const EMultType MultType);
	const TMap<EMultType, int*> Mults{
	{Slide,&SlideMultValue},
		{Jump,&JumpMultValue},
		{Kick,&KickMultValue},
		{Dash,&DashMultValue},
		{Grind,&GrindMultValue},
		{Mantle,&MantleMultValue},
		{Kill,&KillMultValue},
		{Grapple,&GrappleMultValue},
	};
	const  TMap<EMultType, FString> MultString{
		{Slide,"Slide"},
		{Jump,"Jump"},
		{Kick,"Kick"},
		{Dash,"Dash"},
		{Grind,"Grind"},
		{Mantle,"Mantle"},
		{Kill, "Kill"},
		{Empty, "Empty"},
		{Grapple,"Grapple"}
	};
	FORCEINLINE FString ReturnMultString(const EMultType MultType) const{return MultString[MultType];};
};



