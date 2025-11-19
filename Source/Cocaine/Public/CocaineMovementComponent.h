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
	CMOVE_Prone UMETA(DisplayName="Prone"),
	CMOVE_MAX 	UMETA(Hidden),
};
UCLASS()
class COCAINE_API UCocaineMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FSavedMove_Cocaine : public FSavedMove_Character
	{
	public:	
		enum CompressedFlags
		{
			FLAG_Sprint		= 0x10,
			FLAG_Custom_1	= 0x20,
			FLAG_Custom_2	= 0x40,
			FLAG_Custom_3	= 0x80,
		};
		//Flags
		uint8 Saved_bWantsToSprint:1;
		
		//Other Variables
		uint8 Saved_bPrevWantsToCrouch:1;
		uint8 Save_bWantsToProne:1;
		

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
	UPROPERTY(EditDefaultsOnly) float MaxSprintSpeed=750.f;
	
	//slide
	UPROPERTY(EditDefaultsOnly) float MinSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly) float MaxSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly) float SlideEnterImpulse=400.f;
	UPROPERTY(EditDefaultsOnly) float SlideGravityForce=4000.f;
	UPROPERTY(EditDefaultsOnly) float SlideFrictionFactor=.06f;
	UPROPERTY(EditDefaultsOnly) float BrakingDecelerationSliding=1000.f;
	
	//prone
	UPROPERTY(EditDefaultsOnly)float Prone_EnterHoldDuration=2.f;
	UPROPERTY(EditDefaultsOnly)float ProneSlideEnterImpulse=300.f;
	UPROPERTY(EditDefaultsOnly)float ProneMaxSpeed=300.f;
	UPROPERTY(EditDefaultsOnly) float BrakingDecelerationProning=2500.f;
	
	//Transient
	UPROPERTY(Transient) ACocaineCharacter* CocaineCharacterOwner;
	bool Safe_bWantsToSprint;
	bool Safe_bPrevWantsToCrouch;
	bool Safe_bWantsToProne;
	FTimerHandle TimerHandle_EnterProne;
	
public:
	UCocaineMovementComponent();
	//Character Movement Component
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual bool IsMovingOnGround() const override;
    virtual bool CanCrouchInCurrentState() const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
protected:
	virtual void InitializeComponent() override;

	virtual void UpdateFromCompressedFlags(uint8 InFlags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
private:
	//slide
	void EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
	void ExitSlide();
	bool CanSlide() const;
	void PhysSlide(float DeltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
	//Prone
	void TryEnterProne(){Safe_bWantsToSprint = true;}
	UFUNCTION(Server, Reliable) void Server_EnterProne();
	void EnterProne(EMovementMode PrevMode,ECustomMovementMode PrevCustomMode);
	void ExitProne();
	bool CanProne() const;
	void PhysProne(float DeltaTime,int32 Iterations);
public:
	//Interface
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();

	UFUNCTION(BlueprintCallable) void CrouchPressed();
	UFUNCTION(BlueprintCallable) void CrouchReleased();
	
	UFUNCTION(BlueprintCallable) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	UFUNCTION(BlueprintCallable) bool IsMovementMode(EMovementMode InMovementMode) const;
	
};
