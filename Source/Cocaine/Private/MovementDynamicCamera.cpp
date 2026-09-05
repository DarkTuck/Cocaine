// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementDynamicCamera.h"

#include "CocaineCharacter.h"
#include "CocaineMovementComponent.h"

void UMovementDynamicCamera::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!ShouldExecute())
	{
		FieldOfView=MinFOV;
		return;
	}
	const float currentSpeed= MovementComponent->Velocity.Size()/(bUseFixedMaxSpeed?FixedMaxSpeed:MovementComponent->GetMaxSpeed());
	FieldOfView=FMath::Lerp(FieldOfView,FMath::Clamp(MaxFOV*currentSpeed,MinFOV,MaxFOV),FMath::Clamp(DeltaTime*FOVLerpSpeed,0.f,1.f));
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(9,1.f,FColor::Red,FString::Printf(TEXT("Current Speed: %f"),currentSpeed));
	GEngine->AddOnScreenDebugMessage(10,1.f,FColor::Green,FString::Printf(TEXT("Current FOV: %f"),FieldOfView));
#endif
}

void UMovementDynamicCamera::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent=Cast<ACocaineCharacter>(GetOwner())->GetCocaineCharacterMovement();
}

bool UMovementDynamicCamera::ShouldExecute()
{
	if (!MovementComponent) return false;
	if (!bUseDynamicFOV) return false;
	if (bActiveAllMovementModes) return true;
	if (MovementComponent->MovementMode!=MOVE_Custom)
	{
		return MovementModeMap.Find(MovementComponent->MovementMode)!=nullptr;
	}
	return CustomModeMap.Find(MovementComponent->CustomMovementMode)!=nullptr;
}
