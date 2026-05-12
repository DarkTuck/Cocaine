// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CocaineCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CocaineMovementComponent.generated.h"

class AGrindingRail;
class USplineComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashStartDelegate);
/**
 * 
 */
UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None	UMETA(Hidden),
	CMOVE_Slide	UMETA(DisplayName="Slide"),
	CMOVE_Prone UMETA(DisplayName="Prone"),
	CMOVE_Mantle UMETA(DisplayName="Mantle"),
	CMOVE_Grind UMETA(DisplayName="Grind"),
	CMOVE_MAX 	UMETA(Hidden),
};

USTRUCT()
struct FGrindState
{
	GENERATED_BODY()
	
	UPROPERTY() TWeakObjectPtr<AGrindingRail> GrindingRail{nullptr};
	UPROPERTY() TWeakObjectPtr<USplineComponent> GrindSplineComponent{nullptr};
	
	FQuat GrindDetectionRotation{};
	FQuat GrindEntryRotation{};
	FVector GrindDetectionLocation{};
	FVector GrindEntryLocation{};
	float MoveToGrindEntryPointDuration {0.2f};
	float MoveToGrindEntryPointTimeElapsed {0.f};
	float DistanceAlongGrind {0.0f};
	float CurrentGrindSpeed{0.0f};
	bool bGrindingForward {true};
	bool bMovingToGrindEntryPoint {true};
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
			FLAG_Dash		= 0x20,
			FLAG_Kick	= 0x40,
			FLAG_Custom_3	= 0x80,
		};
		// Flags
		uint8 Saved_bWantsToSprint:1;
		uint8 Saved_bWantsToDash:1;
		uint8 Saved_bWantsToKick:1;
		uint8 Saved_bPressedCocaineJump:1;
		
		// Other Variables
		uint8 Saved_bPrevWantsToCrouch:1;
		uint8 Saved_bWantsToProne:1;
		uint8 Saved_bHadAnimRootMotion:1;
		uint8 Saved_bTransitionFinished:1;
		

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
#pragma region Prarameters
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Defaults") float MaxSprintSpeed=750.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Defaults") bool bUseGravityInRootMotion=true;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Defaults") bool bRootMotionDash=false;
	constexpr static float DefaultSpeedBoost = 0.f;
	
	// slide
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Slide") float MinSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Slide") float MaxSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Slide") float SlideEnterImpulse=400.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Slide") float SlideGravityForce=4000.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Slide") float SlideFrictionFactor=.06f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Slide") float BrakingDecelerationSliding=1000.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Slide") float SlideSpeedBoost = DefaultSpeedBoost;
	
	// prone
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Prone") float Prone_EnterHoldDuration=2.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Prone") float ProneSlideEnterImpulse=300.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Prone") float ProneMaxSpeed=300.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Prone") float BrakingDecelerationProning=2500.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Prone") float ProneSpeedBoost = DefaultSpeedBoost;
	
	// Dash
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Dash") float DashImpulse=1000.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Dash") float DashCooldownDuration=1.f;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Dash") float AuthDashCooldownDuration=.9f;
	//Dash RootMotion
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Dash|RootMotion") UAnimMontage* DashMontage;
	
	// Mantle
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MantleMaxDistance = 200;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MantleReachHeight = 50;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MinMantleDepth= 30;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MantleMinWallSteepnessAngle = 75;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MantleMaxSurfaceAngle=40;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MantleMaxAlignmentAngle=45;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MantleMaxSpeed=10000;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle|Animations") FVector MantleTargetOffset=FVector(15,0,15);
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle|Animations") UAnimMontage* TallMantleMontage;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle|Animations") UAnimMontage* TransitionTallMantleMontage;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle|Animations") UAnimMontage* ProxyTallMantleMontage;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle|Animations") UAnimMontage* ShortMantleMontage;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle|Animations") UAnimMontage* TransitionShortMantleMontage;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle|Animations") UAnimMontage* ProxyShortMantleMontage;
	UPROPERTY() FVector MantleTarget;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Mantle") float MantleSpeedBoost = DefaultSpeedBoost;
	
	// Grind
	UPROPERTY(EditAnywhere,Category="MovementSettings|Grind") float GrindDetectionRadius = 50;
	float GrindDetectionRadiusSquared;
	UPROPERTY(EditAnywhere,Category="MovementSettings|Grind",meta=(DeprecatedProperty)) float GrindSpeed = 100;
	UPROPERTY(EditAnywhere,Category="MovementSettings|Grind") float GrindStartingSpeed = 100;
	UPROPERTY(EditAnywhere,Category="MovementSettings|Grind") float GrindMaxSpeed = 1000;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Grind") float GrindSpeedBoost = DefaultSpeedBoost;
	UPROPERTY(EditDefaultsOnly,Category="MovementSettings|Grind") float GrindSpeedGain = 5.f;

	UPROPERTY() FGrindState GrindState{};
	
	// Kick
	UPROPERTY(EditDefaultsOnly, Category="MovementSettings|Kick") float KickForce = 100;
	UPROPERTY(EditDefaultsOnly, Category="MovementSettings|Kick") float KickCooldownDuration = .5f;
	UPROPERTY(EditDefaultsOnly, Category="MovementSettings|kick") float AuthKickCooldownDuration = .4f;
	UPROPERTY(EditDefaultsOnly, Category="MovementSettings|kick") float KickRange=200.f;
#pragma endregion 
#pragma region Transient
	UPROPERTY(Transient) ACocaineCharacter* CocaineCharacterOwner;
	
	// Flags
	bool Safe_bWantsToSprint;
	bool Safe_bWantsToProne;
	bool Safe_bWantsToDash;
	bool Safe_bWantsToKick;
	
	bool Safe_bHadAnimRootMotion;
	bool Safe_bPrevWantsToCrouch;
	float DashStartTime;
	float KickStartTime;
	FTimerHandle TimerHandle_EnterProne;
	FTimerHandle TimerHandle_DashCooldown;
	FTimerHandle TimerHandle_KickCooldown;
	
	bool Safe_bTransitionFinished;
	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;
	UPROPERTY(Transient) UAnimMontage*TransitionQueuedMontage;
	float TransitionQueuedMontageSpeed;
	int TransitionRMS_ID;
#pragma endregion

	// Replication
	UPROPERTY(ReplicatedUsing=OnRep_DashStart) bool Proxy_bDashStart;
	UPROPERTY(ReplicatedUsing=OnRep_ShortMantle) bool Proxy_bShortMantle;
	UPROPERTY(ReplicatedUsing=OnRep_TallMantle) bool Proxy_bTallMantle;
	UPROPERTY(ReplicatedUsing=OnRep_Kick) bool Proxy_bKick;
	

	// Delegates
public:
	UPROPERTY(BlueprintAssignable) FDashStartDelegate DashStartDelegate;
	
public:
	UCocaineMovementComponent();
	virtual void BeginPlay() override;
	// Character Movement Component
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual bool IsMovingOnGround() const override;
    virtual bool CanCrouchInCurrentState() const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	float GetSpeedBoost() const;
	virtual bool CanAttemptJump() const override;
	virtual  void AddInputVector(FVector WorldVector, bool bForce = false) override;
protected:
	virtual void InitializeComponent() override;
	virtual void UpdateFromCompressedFlags(uint8 InFlags) override;

public:	
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
private:
	void UpdateMult() const;
	// slide	
private:
	void EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
	void ExitSlide();
	bool CanSlide() const;
	void PhysSlide(float DeltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
	
	// Prone
private:
	void TryEnterProne(){Safe_bWantsToSprint = true;}
	UFUNCTION(Server, Reliable) void Server_EnterProne();
	void EnterProne(EMovementMode PrevMode,ECustomMovementMode PrevCustomMode);
	void ExitProne();
	bool CanProne() const;
	void PhysProne(float DeltaTime,int32 Iterations);
	
	// Dash
private:
	void OnDashCooldownFinished();
	bool CanDash() const;
	void PerformDash();
	void PerformDashRootMotion();
	
	// Mantle
private:
	bool TryMantle();
	void EnterMantle(EMovementMode PrevMode,ECustomMovementMode PrevCustomMode);
	void ExitMantle();
	void PhysMantle(float DeltaTime,int32 Iterations);
	FVector GetMantleStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit, bool bTallMantle) const;
	
	// Grinding
private:
	bool TryGrind();
	void EnterGrind(EMovementMode PrevMode,ECustomMovementMode PrevCustomMode);
	void ExitGrind();
	void PhysGrind(float DeltaTime,int32 Iterations);
	
	// Kick
private:
	void OnKickCooldownFinished();
	bool CanKick() const;
	void PerformKick();
	
	// Flying
public:
	FORCEINLINE void SetFlying(const bool Set) {SetMovementMode(Set?MOVE_Flying:MOVE_Walking);}
	
	//Helpers
private:
	bool IsServer() const;
	float CapR() const; // Get Capsule radius
	float CapHH() const; // Get Capsule half height
	double GetTS() const; // Get Time Seconds

	// Interface
public:
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();

	UFUNCTION(BlueprintCallable) void CrouchPressed();
	UFUNCTION(BlueprintCallable) void CrouchReleased();
	
	UFUNCTION(BlueprintCallable) void DashPressed();
	UFUNCTION(BlueprintCallable) void DashReleased();
	
	UFUNCTION(BlueprintCallable) void KickPressed();
	UFUNCTION(BlueprintCallable) void KickReleased();
	
	UFUNCTION(BlueprintCallable) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	UFUNCTION(BlueprintCallable) bool IsMovementMode(EMovementMode InMovementMode) const;
	
	// Proxy Replication
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UFUNCTION() void OnRep_DashStart();
	UFUNCTION() void OnRep_ShortMantle();
	UFUNCTION() void OnRep_TallMantle();
	UFUNCTION() void OnRep_Kick();
	
	
};