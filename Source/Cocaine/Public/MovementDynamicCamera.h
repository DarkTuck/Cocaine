// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CocaineMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "MovementDynamicCamera.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API UMovementDynamicCamera : public UCameraComponent
{
	GENERATED_BODY()
	public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV",meta=(DisplayName="Use Dynamic FOV"))
	bool bUseDynamicFOV = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV",meta=(DisplayName="Active All Movement Modes",editcondition="bUseDynamicFOV"))
	bool bActiveAllMovementModes = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV", meta=(DisplayName="Use Fixed Max Speed",editcondition="bUseDynamicFOV"))
	bool bUseFixedMaxSpeed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV",meta=(editcondition="bUseDynamicFOV"))
	float MinFOV = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV",meta=(editcondition="bUseDynamicFOV"))
	float MaxFOV = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV",meta=(editcondition="bUseDynamicFOV"))
	float FOVLerpSpeed = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV", meta=(DisplayAfter="bUseFixedMaxSpeed",editcondition="bUseFixedMaxSpeed"))
	float FixedMaxSpeed = 1600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV", DisplayName="Movemnt Modes To use dyanmicFOV" ,meta=(editcondition="bUseFixedMaxSpeed"))
	TMap<TEnumAsByte<EMovementMode>,bool> MovementModeMap
	{
		{MOVE_None,false},
		{MOVE_Walking,true},
		{MOVE_Falling,true},
		{MOVE_Swimming,false},
		{MOVE_Flying,false},
		{MOVE_Custom,true},
		{MOVE_MAX,false},
	};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dynamic FOV", DisplayName="Custom Modes To use dynamicFOV",meta=(editcondition="bUseDynamicFOV"))
	TMap<uint8,bool> CustomModeMap{
		{CMOVE_None,false},
		{CMOVE_Slide,true},
		{CMOVE_Prone,true},
		{CMOVE_Mantle,true},
		{CMOVE_Grind,true},
		{CMOVE_Grapple,true},
		{CMOVE_WallRun,true},
		{CMOVE_MAX,false},
	};
private:
	UCocaineMovementComponent* MovementComponent;
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	bool ShouldExecute();
};
