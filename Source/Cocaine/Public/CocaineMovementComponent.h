// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CocaineCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CocaineMovementComponent.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None	UMETA(Hidden),
	CMOVE_Slide	UMETA(DisplayName="Slide"),
	CMOVE_MAX 	UMETA(Hidden),
};
UCLASS()
class COCAINE_API UCocaineMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FSavedMove_Cocaine : public FSavedMove_Character
	{
		//Flag
		uint8 Saved_bWantsToSprint:1;

		uint8 Saved_bPrevWantsToCrouch:1;
		
	public:
		FSavedMove_Cocaine();
		typedef FSavedMove_Character Super;
		
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
	//Parameters
	UPROPERTY(EditDefaultsOnly)float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly)float Walk_MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly)float Slide_MinSpeed=350;
	UPROPERTY(EditDefaultsOnly)float Slide_EnterImpulse=500;
	UPROPERTY(EditDefaultsOnly)float Slide_GravityForce=5000;
	UPROPERTY(EditDefaultsOnly)float Slide_Friction=1.3;
	//Transient
	UPROPERTY(Transient) ACocaineCharacter* CocaineCharacterOwner;
	
	bool Safe_bWantsToSprint;
	bool Safe_bPrevWantsToCrouch;
public:
	UCocaineMovementComponent();
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual bool IsMovingOnGround() const override;
    virtual bool CanCrouchInCurrentState() const override;
protected:
	virtual void InitializeComponent() override;

	virtual void UpdateFromCompressedFlags(uint8 InFlags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
private:
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float DeltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
public:
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();

	UFUNCTION(BlueprintCallable) void CrouchPressed();
	UFUNCTION(BlueprintCallable) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	
};
