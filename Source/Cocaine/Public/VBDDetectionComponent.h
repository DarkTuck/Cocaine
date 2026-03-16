// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VBDDetectionComponent.generated.h"


UCLASS(ClassGroup=(ViewBasedDanger), meta=(BlueprintSpawnableComponent))
class COCAINE_API UVBDDetectionComponent : public UActorComponent
{
	GENERATED_BODY()
	ACharacter* Character;

public:
	// Sets default values for this component's properties
	UVBDDetectionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnComponentCreated() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
