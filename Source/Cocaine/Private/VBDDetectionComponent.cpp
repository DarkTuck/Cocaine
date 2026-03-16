// Fill out your copyright notice in the Description page of Project Settings.


#include "VBDDetectionComponent.h"

#include "CocaineCharacter.h"
#include "GameFramework/Character.h"
#include "CollisionLibrary.h"


// Sets default values for this component's properties
UVBDDetectionComponent::UVBDDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVBDDetectionComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
}

void UVBDDetectionComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	Character = Cast<ACharacter>( GetOwner());
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("VBDDetectionComponent: Character not found"));
	}
}


// Called every frame
void UVBDDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	// ...
}

