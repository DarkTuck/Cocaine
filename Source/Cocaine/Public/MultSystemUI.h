// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultSystemUI.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API  UMultSystemUI : public UUserWidget
{
public:
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateScore(const int score);
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateMult(const int mult);
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateStoredMult(const float storedMult);
	UFUNCTION(BlueprintImplementableEvent, Category="MultSystem")
	void BP_UpdateLastAction(const FString& Action);
	
	GENERATED_BODY()
};
