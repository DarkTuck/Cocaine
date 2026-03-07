// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CocaineGameMode.h"
#include "Blueprint/UserWidget.h"
#include "MultSystemUI.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API UMultSystemUI : public UUserWidget
{
public:
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateScore(int score);
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateMult(int mult);
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateStoredMult(int storedMult);
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateLastAction(EMultType multType);
	
	GENERATED_BODY()
};
