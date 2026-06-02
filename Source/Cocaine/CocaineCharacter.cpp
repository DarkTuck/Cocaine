// Copyright Epic Games, Inc. All Rights Reserved.

#include "CocaineCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Cocaine.h"
#include "Public/CocaineMovementComponent.h"
#include "CableComponent.h"
#include "CocaineGameMode.h"

void ACocaineCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACocaineCharacter, HeadScale);
}

ACocaineCharacter::ACocaineCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UCocaineMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CocaineMovementComponent=Cast<UCocaineMovementComponent>(GetCharacterMovement());
	CocaineMovementComponent->SetIsReplicated(true);
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	GrappleCable=CreateDefaultSubobject<UCableComponent>(TEXT("GrappleCable"));
	GrappleCable->SetupAttachment(FirstPersonCameraComponent);
	GrappleCable->SetVisibility(false);
}

void ACocaineCharacter::Jump()
{
	Super::Jump();
	bPressedCocaineJump=true;
	bPressedJump=false;
}

void ACocaineCharacter::StopJumping()
{
	Super::StopJumping();
	bPressedCocaineJump=false;
}

void ACocaineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACocaineCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACocaineCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACocaineCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACocaineCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACocaineCharacter::LookInput);
		
		// SlowMo
		EnhancedInputComponent->BindAction(SlowMoAction, ETriggerEvent::Started,this,&ACocaineCharacter::SlowMo);
		EnhancedInputComponent->BindAction(SlowMoAction, ETriggerEvent::Completed,this,&ACocaineCharacter::StopSlowMo);
		
		// Interact
		EnhancedInputComponent->BindAction(InteractAction,ETriggerEvent::Triggered,this,&ACocaineCharacter::Interact);
		EnhancedInputComponent->BindAction(InteractAction,ETriggerEvent::Completed,this,&ACocaineCharacter::StopInteract);
		
		// Kick
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Started,this,&ACocaineCharacter::PerformKick);
		EnhancedInputComponent->BindAction(KickAction,ETriggerEvent::Completed,this,&ACocaineCharacter::StopKick);
		
		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction,ETriggerEvent::Started,this,&ACocaineCharacter::PerformCrouch);
		EnhancedInputComponent->BindAction(CrouchAction,ETriggerEvent::Completed,this,&ACocaineCharacter::StopCrouch);
		
		// Sprint
		EnhancedInputComponent->BindAction(SprintAction,ETriggerEvent::Started,this,&ACocaineCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction,ETriggerEvent::Completed,this,&ACocaineCharacter::StopSprint);
		
		// Dash
		EnhancedInputComponent->BindAction(DashAction,ETriggerEvent::Started,this,&ACocaineCharacter::Dash);
		EnhancedInputComponent->BindAction(DashAction,ETriggerEvent::Completed,this,&ACocaineCharacter::StopDash);
	}
	else
	{
		UE_LOG(LogCocaine, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACocaineCharacter::BeginPlay()
{
	Super::BeginPlay();
	CocaineGameMode=Cast<ACocaineGameMode>(GetWorld()->GetAuthGameMode());
	CocaineMovementComponent->SetGrappleCable(GrappleCable);
}

void ACocaineCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsGrappling)
	{
		GrappleCable->EndLocation=GetActorTransform().InverseTransformPosition(GrapplingPoint);
		FVector GrappleDirection=GrapplingPoint-GetActorLocation();
		GrappleDirection.Normalize();
		GrappleDirection*=GrappleForce;
		FVector Correction = GetLastMovementInputVector();
		Correction*=GrappleSteeringForce;
		FVector Direction = GrappleDirection+Correction;
		GetCharacterMovement()->AddForce(Direction);
		//GetCharacterMovement()->AddForce((GrapplingPoint-GetActorLocation()).GetSafeNormal()*GrappleForce);
	}
}

FCollisionQueryParams ACocaineCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}

void ACocaineCharacter::Interact()
{
	const FVector Start{GetCapsuleComponent()->GetComponentLocation()};
	const FVector End{Start+(MaxLineDistance*FirstPersonCameraComponent->GetForwardVector())};
	DrawDebugLine(GetWorld(),Start,End,FColor::Emerald);
	
	FHitResult Hit;
	if (const bool bHasHit {GetWorld()->SweepSingleByChannel(Hit,Start,End,FQuat::Identity,ECC_GameTraceChannel2,FCollisionShape::MakeSphere(100.f))})
	{
		bIsGrappling=true;
		Cast<ACocaineGameMode>(GetWorld()->GetAuthGameMode())->AddMult(Grapple);
		GetCocaineCharacterMovement()->SetFlying(true);
		GrappleCable->SetVisibility(true);
		GrapplingPoint=Hit.ImpactPoint;
	}
}

void ACocaineCharacter::StopInteract()
{
	bIsGrappling=false;
	if (!GetCharacterMovement()->IsFalling())
	{
		GetCocaineCharacterMovement()->SetFlying(false);
	}
	GrappleCable->SetVisibility(false);
}

void ACocaineCharacter::PerformKick()
{
	GetCocaineCharacterMovement()->KickPressed();
}

void ACocaineCharacter::StopKick()
{
	GetCocaineCharacterMovement()->KickReleased();	
}

void ACocaineCharacter::PerformCrouch()
{
	GetCocaineCharacterMovement()->CrouchPressed();
}
void ACocaineCharacter::StopCrouch()
{
	GetCocaineCharacterMovement()->CrouchReleased();
}

void ACocaineCharacter::Sprint()
{
	GetCocaineCharacterMovement()->SprintPressed();
}

void ACocaineCharacter::StopSprint()
{
	GetCocaineCharacterMovement()->SprintReleased();
}

void ACocaineCharacter::Dash()
{
	GetCocaineCharacterMovement()->DashPressed();
}

void ACocaineCharacter::StopDash()
{
	GetCocaineCharacterMovement()->DashReleased();
}

void ACocaineCharacter::SlowMo()
{
	CocaineGameMode->StartSlowMo();	
}

void ACocaineCharacter::StopSlowMo()
{
	CocaineGameMode->StopSlowMo();
}

/* Most of this code is "borrowed" from UT3 as we weren't able to do headshots using line tracing.
 * As UT3 uses math for this ability, and for now as I am not entirely sure how it's working,
 * the code is copied/updated for the newer engine while cutting out stuff we don't use/need
 * When I get better understanding, then maybe this code will be more unique for our purpose */

FVector ACocaineCharacter::GetHeadLocation(float PredictionTime)
{
	// force mesh update if necessary
	if (GetMesh()->IsRegistered() && GetMesh()->VisibilityBasedAnimTickOption > EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones && !GetMesh()->bRecentlyRendered)
	{
		if (GetMesh()->VisibilityBasedAnimTickOption > EVisibilityBasedAnimTickOption::AlwaysTickPose)
		{
			// important to have significant time here so any transitions complete
			// FIXME: step size needs to be this small due to usage of framerate-dependent FInterpTo() in the anim blueprint
			constexpr float Step = 0.1f;
			for (float TickTime = FMath::Min<float>(GetWorld()->TimeSeconds - GetMesh()->GetLastRenderTime(), 1.0f); TickTime > 0.0f; TickTime -= Step)
			{
				GetMesh()->TickAnimation(FMath::Min<float>(TickTime, Step), false);
			}
			GetMesh()->AnimUpdateRateParams->bSkipEvaluation = false;
			GetMesh()->AnimUpdateRateParams->bInterpolateSkippedFrames = false;
			GetMesh()->RefreshBoneTransforms();
			GetMesh()->UpdateComponentToWorld();
		}
	}
	const FVector Result = GetMesh()->GetSocketLocation(HeadBone) + FVector(0.0f, 0.0f, HeadHeight);
	
	// offset based on PredictionTime to previous position
	return Result;
}

bool ACocaineCharacter::IsHeadShot(FVector HitLocation, FVector ShotDirection, float WeaponHeadScaling,
	AActor* ShotInstigator, float PredictionTime)
{
	const FVector HeadLocation = GetHeadLocation();
	const bool bHeadShot = FMath::PointDistToLine(HeadLocation, ShotDirection, HitLocation) < HeadRadius * HeadScale * WeaponHeadScaling;
	
	return bHeadShot;
}

/* End off UT3 borrowed code */

void ACocaineCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ACocaineCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ACocaineCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACocaineCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ACocaineCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ACocaineCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}
