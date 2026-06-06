// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CocaineGameMode.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Net/UnrealNetwork.h"
#include "CocaineCharacter.generated.h"

struct FTimeline;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class ACocaineCharacter : public ACharacter
{
	GENERATED_BODY()
	UPROPERTY()
	ACocaineGameMode* CocaineGameMode;

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category=Movement) class UCocaineMovementComponent*CocaineMovementComponent;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;
	
	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* CrouchAction;
	
	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;
	
	/** Kick Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* KickAction;
	
	/** Dash Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* DashAction;
	
	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
	/** Slow motion Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SlowMoAction;
	
	/** Grappling's/Interact Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* InteractAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category ="Grappling")
	class UCableComponent* GrappleCable;
	const FVector StartingHeadLocation= FVector(-2.8f, 5.89f, 0.0f);
public:
	bool bPressedCocaineJump;
public:
	ACocaineCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void Jump() override;
	virtual void StopJumping() override;

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void BeginPlay() override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UCocaineMovementComponent* GetCocaineCharacterMovement() const {return CocaineMovementComponent;}

	FCollisionQueryParams GetIgnoreCharacterParams() const;
	
	UFUNCTION(BlueprintPure)
	FVector GetStartingHeadLocation() const {return StartingHeadLocation;}
protected:
	virtual void PerformGrapple();
	virtual void StopGrapple();
	
	virtual void PerformKick();
	virtual void StopKick();
	
	virtual void PerformCrouch();
	virtual void StopCrouch();
	
	virtual void Sprint();
	virtual void StopSprint();
	
	virtual void Dash();
	virtual void StopDash();
	
	virtual void SlowMo();
	virtual void StopSlowMo();
	
	bool bIsGrappling{false};
	FVector GrapplingPoint;
protected:
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Grappling")
	float GrappleForce{1000000.f};
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Grappling")
	float GrappleSteeringForce{500000.f};
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Grappling")
	float MaxLineDistance {1000.f};
	
	
	// Headshots
protected:
	
	/** head bone/socket for headshots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pawn)
	FName HeadBone = "head";
	/** head Z offset from head bone */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pawn)
	float HeadHeight;
	/** radius around head location that counts as headshot at 1.0 head scaling */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pawn)
	float HeadRadius;
	/** head scale factor (generally for use at runtime) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Pawn)
	float HeadScale;
	
public:
	/** returns location of head (origin of headshot zone); will force a skeleton update if mesh hasn't been rendered (or dedicated server) so the provided position is accurate */
	virtual FVector GetHeadLocation(float PredictionTime=0.f);
	/** checks for a head shot - called by weapons with head shot bonuses. */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	virtual bool IsHeadShot(FVector HitLocation, FVector ShotDirection, float WeaponHeadScaling, AActor* ShotInstigator, float PredictionTime = 0.f);
};