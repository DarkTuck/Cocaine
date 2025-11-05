// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CocainePlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API ACocainePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly) float CrouchBlendDuration=.5f;
	float CrouchBlendTime;
public:
	ACocainePlayerCameraManager();
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;	
};
