// Fill out your copyright notice in the Description page of Project Settings.


#include "CocaineCashGameInstanceSubsystem.h"

void UCocaineCashGameInstanceSubsystem::AddCash(const int Amount)
{
	Cash+=Amount;
	if (WITH_EDITOR) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Cash: %d"),Cash));
}
