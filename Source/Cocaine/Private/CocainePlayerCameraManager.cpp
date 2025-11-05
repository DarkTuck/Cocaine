// Fill out your copyright notice in the Description page of Project Settings.
#include "CocainePlayerCameraManager.h"
#include "CocaineCharacter.h"
#include "CocaineMovementComponent.h"
#include "Components/CapsuleComponent.h"

ACocainePlayerCameraManager::ACocainePlayerCameraManager()
{
}
//this function makes camera move smoothly when starting to crouch
void ACocainePlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
	if (ACocaineCharacter* CocaineCharacter = Cast<ACocaineCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		UCocaineMovementComponent* CMC = CocaineCharacter->GetCocaineCharacterMovement();
		FVector TargetCrouchOffset = FVector(0, 0, CMC->GetCrouchedHalfHeight()-CocaineCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector Offset = FMath::Lerp(FVector::ZeroVector,TargetCrouchOffset,FMath::Clamp(CrouchBlendTime/CrouchBlendDuration,0.f,1.f));
		if (CMC->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime+DeltaTime,0.f,CrouchBlendDuration);
			Offset-=TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime=FMath::Clamp(CrouchBlendTime-DeltaTime,0.f,CrouchBlendDuration);
		}
		if (CMC->IsMovingOnGround())
		{
			OutVT.POV.Location += Offset;
		}
	}
}