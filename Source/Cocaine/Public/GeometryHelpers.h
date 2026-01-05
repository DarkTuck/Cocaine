// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeometryHelpers.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API UGeometryHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static  USplineComponent* CreateOffsettedSpline(USplineComponent* Dest, const USplineComponent* Source, FVector Offset, float CornerRadius);
};
