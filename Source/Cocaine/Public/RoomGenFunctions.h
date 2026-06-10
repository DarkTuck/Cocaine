// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoomGenFunctions.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API URoomGenFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void GenerateNavMesh(FVector Location, FVector Extent, FRotator Rotation = FRotator::ZeroRotator);
	
};
