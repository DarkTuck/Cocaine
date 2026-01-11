// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "GrindingRail.generated.h"


/**
 * 
 */
UCLASS(BlueprintType)
class COCAINE_API AGrindingRail : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()
	AGrindingRail();
	virtual void OnConstruction(const FTransform& Transform) override;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spline") USplineComponent* Spline;
	FORCEINLINE USplineComponent* GetGrindRail() const { return Spline; }
};
