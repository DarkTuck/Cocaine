// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "DynamicMeshActor.h"
#include "GeometryScript/GeometryScriptTypes.h"
#include "GrindingRail.generated.h"


/**
 * 
 */
UCLASS(BlueprintType)
class COCAINE_API AGrindingRail : public ADynamicMeshActor
{
	GENERATED_BODY()
public:
 	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spline") USplineComponent* Spline;
 	
 	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RailSettings") float Radius = 5.f;
 	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RailSettings") float CornerRadius = 42.f;
 	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RailSettings") int Roundness = 8;
 	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RailSettings") int Subdivision = 3;
 	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="RailSettings") float Height = 50.f;
protected:	
	FGeometryScriptPolyPath Brush{};
public:
	UFUNCTION(BlueprintPure)
 	FORCEINLINE USplineComponent* GetGrindRail() const { return Spline; }
	UFUNCTION(BlueprintCallable)
	void RebuildMesh();
protected:
	AGrindingRail();
	virtual void OnConstruction(const FTransform& Transform) override;
};
