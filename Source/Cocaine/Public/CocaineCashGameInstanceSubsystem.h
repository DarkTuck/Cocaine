// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CocaineCashGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API UCocaineCashGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	int Cash = 0;
	int LastMultValue = 1;
	
public:
	UFUNCTION(BlueprintCallable)
	int GetCash() const {return Cash;};
	UFUNCTION(BlueprintCallable)
	void AddCash(const int Amount);
	UFUNCTION(BlueprintCallable)
	int GetLastMultValue() const {return LastMultValue;};
	UFUNCTION(BlueprintCallable)
	void SetLastMultValue(const int NewMultValue){LastMultValue=NewMultValue;};
};
