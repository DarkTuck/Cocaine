// Fill out your copyright notice in the Description page of Project Settings.


#include "GrindingRail.h"

#include "Components/SplineComponent.h"

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
}
