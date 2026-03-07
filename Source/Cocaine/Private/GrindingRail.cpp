// Fill out your copyright notice in the Description page of Project Settings.


#include "GrindingRail.h"

#include "GeometryHelpers.h"
#include "Components/SplineComponent.h"
#include "GeometryScript/PolygonFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"

AGrindingRail::AGrindingRail()
{	
	if (UDynamicMeshComponent* DynamicMeshComp = GetDynamicMeshComponent())
	{
		DynamicMeshComp->SetCollisionProfileName(FName{"Grind"});
	}
}

void AGrindingRail::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!Spline)
	{
		Spline = NewObject<USplineComponent>(this, USplineComponent::StaticClass(),TEXT("Spline"));
		Spline->RegisterComponent();
		Spline->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
		Spline->bEditableWhenInherited = true;
	}
	RebuildMesh();
}

void AGrindingRail::RebuildMesh()
{
	UDynamicMeshComponent* MeshComponent = Cast<UDynamicMeshComponent>(GetComponentByClass(UDynamicMeshComponent::StaticClass()));
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No DynamicMeshComponent found"));
		return;
	}
	const FVector2D Center(0,0);
	
	Brush=UGeometryScriptLibrary_PolyPathFunctions::CreateCirclePath2D(Center,Radius,Roundness);
	
	USplineComponent* HelperSpline = NewObject<USplineComponent>(this,USplineComponent::StaticClass());
	const FVector Offset(0,0,Height);
	UGeometryHelpers::CreateOffsettedSpline(HelperSpline, Spline,Offset, CornerRadius);
	
	TArray<FTransform> Frames{};
	TArray<double> FrameTimes{};
	const int32 NumSamples{((1+Subdivision)*(HelperSpline->GetNumberOfSplinePoints()-1))+1};

	const FGeometryScriptSplineSamplingOptions SamplingOptions{NumSamples};
	const FTransform Transform;
	UGeometryScriptLibrary_PolyPathFunctions::SampleSplineToTransforms(HelperSpline,Frames,FrameTimes,SamplingOptions,Transform);

	constexpr FGeometryScriptPrimitiveOptions Options{};

	UDynamicMesh* Mesh = MeshComponent->GetDynamicMesh();
	Mesh->Reset();
	
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendSweepPolygon(Mesh,
		Options,
		Transform,
		UGeometryScriptLibrary_PolyPathFunctions::Conv_GeometryScriptPolyPathToArrayOfVector2D(Brush),
		Frames);
	HelperSpline->DestroyComponent();
	MeshComponent->MarkRenderStateDirty();
}
