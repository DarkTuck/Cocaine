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
			FLAG_Custom_2	= 0x40,
			FLAG_Custom_3	= 0x80,
		};
		// Flags
		uint8 Saved_bWantsToSprint:1;
		uint8 Saved_bWantsToDash:1;
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
	UPROPERTY(EditDefaultsOnly) float MaxSprintSpeed=750.f;
	UPROPERTY(EditDefaultsOnly) bool bUseGravityInRootMotion=true;
	UPROPERTY(EditDefaultsOnly) bool bRootMotionDash=false;
	
	// slide
	UPROPERTY(EditDefaultsOnly) float MinSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly) float MaxSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly) float SlideEnterImpulse=400.f;
	UPROPERTY(EditDefaultsOnly) float SlideGravityForce=4000.f;
	UPROPERTY(EditDefaultsOnly) float SlideFrictionFactor=.06f;
	UPROPERTY(EditDefaultsOnly) float BrakingDecelerationSliding=1000.f;
	
	// prone
	UPROPERTY(EditDefaultsOnly) float Prone_EnterHoldDuration=2.f;
	UPROPERTY(EditDefaultsOnly) float ProneSlideEnterImpulse=300.f;
	UPROPERTY(EditDefaultsOnly) float ProneMaxSpeed=300.f;
	UPROPERTY(EditDefaultsOnly) float BrakingDecelerationProning=2500.f;
	
	// Dash
	UPROPERTY(EditDefaultsOnly) float DashImpulse=1000.f;
	UPROPERTY(EditDefaultsOnly) float DashCooldownDuration=1.f;
	UPROPERTY(EditDefaultsOnly) float AuthDashCooldownDuration=.9f;
	//Dash RootMotion
	UPROPERTY(EditDefaultsOnly) UAnimMontage* DashMontage;
	
	// Mantle
	UPROPERTY(EditDefaultsOnly) float MantleMaxDistance = 200;
	UPROPERTY(EditDefaultsOnly) float MantleReachHeight = 50;
	UPROPERTY(EditDefaultsOnly) float MinMantleDepth= 30;
	UPROPERTY(EditDefaultsOnly) float MantleMinWallSteepnessAngle = 75;
	UPROPERTY(EditDefaultsOnly) float MantleMaxSurfaceAngle=40;
	UPROPERTY(EditDefaultsOnly) float MantleMaxAlignmentAngle=45;
	UPROPERTY(EditDefaultsOnly) float MantleMaxSpeed=10000;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* TallMantleMontage;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* TransitionTallMantleMontage;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* ProxyTallMantleMontage;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* ShortMantleMontage;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* TransitionShortMantleMontage;
	UPROPERTY(EditDefaultsOnly) UAnimMontage* ProxyShortMantleMontage;
	UPROPERTY() FVector MantleTarget;
	
	// Grind
	UPROPERTY(EditAnywhere) float GrindDetectionRadius = 50;
	UPROPERTY() FGrindState GrindState{};
#pragma endregion 
#pragma region Transient
	UPROPERTY(Transient) ACocaineCharacter* CocaineCharacterOwner;
	
	// Flags
	bool Safe_bWantsToSprint;
	bool Safe_bWantsToProne;
	bool Safe_bWantsToDash;
	
	bool Safe_bHadAnimRootMotion;
	bool Safe_bPrevWantsToCrouch;
	float DashStartTime;
	FTimerHandle TimerHandle_EnterProne;
	FTimerHandle TimerHandle_DashCooldown;
	
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
	
	// Flying
public:
	FORCEINLINE void SetFlying(const bool Set) {SetMovementMode(Set?MOVE_Flying:MOVE_Walking);}
	
	//Helpers
private:
	bool IsServer() const;
	float CapR() const; // Get Capsule radius
	float CapHH() const; // Get Capsule half height

	// Interface
public:
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();

	UFUNCTION(BlueprintCallable) void CrouchPressed();
	UFUNCTION(BlueprintCallable) void CrouchReleased();
	
	UFUNCTION(BlueprintCallable) void DashPressed();
	UFUNCTION(BlueprintCallable) void DashReleased();
	
	UFUNCTION(BlueprintCallable) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	UFUNCTION(BlueprintCallable) bool IsMovementMode(EMovementMode InMovementMode) const;
	
	// Proxy Replication
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UFUNCTION() void OnRep_DashStart();
	UFUNCTION() void OnRep_ShortMantle();
	UFUNCTION() void OnRep_TallMantle();
	
	
};