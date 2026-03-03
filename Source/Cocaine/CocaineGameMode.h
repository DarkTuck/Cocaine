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
	Mantle
};
USTRUCT()
struct FCurrents
{
	GENERATED_BODY()
	float currentScore;
	float currentMult;
};

UCLASS(abstract)
class ACocaineGameMode : public AGameModeBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,Category="Mult System") float SlideMultValue=2.f;
	UPROPERTY(EditDefaultsOnly,Category="Mult System") float JumpMultValue=2.f;
	UPROPERTY(EditDefaultsOnly,Category="Mult System") float KickMultValue=2.f;
	UPROPERTY(EditDefaultsOnly,Category="Mult System") float DashMultValue=2.f;
	UPROPERTY(EditDefaultsOnly,Category="Mult System") float GrindMultValue=2.f;
	UPROPERTY(EditDefaultsOnly,Category="Mult System") float MantleMultValue=2.f;
	UPROPERTY(EditDefaultsOnly,Category="Mult System") float MultFadeDuration=5.f;
	
	float SavedScore;
	FCurrents Currents{0,1};
	FTimerHandle MultFade;
	EMultType CurrentMultType;
	void UpdateScore(const float& Score);
	void OnMultFade();
	
	void RestartMultFade();
	void StopMultFade();

	
public:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	ACocaineGameMode();
	UFUNCTION(BlueprintCallable)
	void AddMult(const EMultType MultType);
private:
	void CreateMults();
	TMap<EMultType,float> Mults{};	
};



