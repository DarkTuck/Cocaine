// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CocaineMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class COCAINE_API UCocaineMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FSavedMove_Cocaine : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;
		uint8 Saved_bWantsToSprint:1;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class  FNetworkPredictionData_Client_Cocaine : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Cocaine(const UCharacterMovementComponent& ClientMovement);
		typedef FNetworkPredictionData_Client_Character Super;
		virtual FSavedMovePtr AllocateNewMove() override;
	};
	UPROPERTY(EditDefaultsOnly)float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly)float Walk_MaxWalkSpeed;
	
	bool Safe_bWantsToSprint;
public:
	UCocaineMovementComponent();
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
protected:
	virtual void UpdateFromCompressedFlags(uint8 InFlags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
public:
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();
	
};
