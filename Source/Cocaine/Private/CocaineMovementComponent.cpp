// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/CocaineMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#pragma region Saved Move
UCocaineMovementComponent::FSavedMove_Cocaine::FSavedMove_Cocaine()
{
	Saved_bWantsToSprint=0;
}

bool UCocaineMovementComponent::FSavedMove_Cocaine::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Cocaine* NewCocaineMove = static_cast<FSavedMove_Cocaine*>(NewMove.Get());
	if (Saved_bWantsToSprint != NewCocaineMove->Saved_bWantsToSprint)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UCocaineMovementComponent::FSavedMove_Cocaine::Clear()
{
	FSavedMove_Character::Clear();
	Saved_bWantsToSprint = 0;
}

uint8 UCocaineMovementComponent::FSavedMove_Cocaine::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (Saved_bWantsToSprint) Result |=  FLAG_Custom_0;
	return Result;
}

void UCocaineMovementComponent::FSavedMove_Cocaine::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	const UCocaineMovementComponent* CharacterMovement=Cast<UCocaineMovementComponent>(C->GetCharacterMovement());
	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch=CharacterMovement->Safe_bPrevWantsToCrouch;
}

void UCocaineMovementComponent::FSavedMove_Cocaine::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	UCocaineMovementComponent* CharacterMovement=Cast<UCocaineMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch=Saved_bPrevWantsToCrouch;
}
#pragma endregion
#pragma region Client Network Prediction Data
UCocaineMovementComponent::FNetworkPredictionData_Client_Cocaine::FNetworkPredictionData_Client_Cocaine(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr UCocaineMovementComponent::FNetworkPredictionData_Client_Cocaine::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Cocaine());
}
#pragma endregion
FNetworkPredictionData_Client* UCocaineMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);
	if (ClientPredictionData== nullptr)
	{
		UCocaineMovementComponent*MutableThis=const_cast<UCocaineMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Cocaine(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist=92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist=140.f;
	}
	return ClientPredictionData;
}

void UCocaineMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CocaineCharacterOwner=Cast<ACocaineCharacter>(GetOwner());
}

void UCocaineMovementComponent::UpdateFromCompressedFlags(uint8 InFlags)
{
	Super::UpdateFromCompressedFlags(InFlags);

	Safe_bWantsToSprint =(InFlags & FSavedMove_Character::FLAG_Custom_0)!=0;
}

void UCocaineMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	//Makes Sprint Possible
	if (MovementMode==MOVE_Walking)
	{
		MaxWalkSpeed = Safe_bWantsToSprint? Sprint_MaxWalkSpeed:Walk_MaxWalkSpeed;
	}
	Safe_bPrevWantsToCrouch=bWantsToCrouch;
}

bool UCocaineMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround()||IsCustomMovementMode(CMOVE_Slide);
}

bool UCocaineMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState()&&IsMovingOnGround();
}

void UCocaineMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (MovementMode==MOVE_Walking&&!bWantsToCrouch&&Safe_bPrevWantsToCrouch)
	{
		FHitResult PotentialSlideSurface;
		if (Velocity.SizeSquared()>pow(Slide_MinSpeed,2)&&GetSlideSurface(PotentialSlideSurface))
		{
			EnterSlide();
		}
	}

	if (IsCustomMovementMode(CMOVE_Slide)&&!bWantsToCrouch)
	{
		ExitSlide();
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UCocaineMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp,Fatal,TEXT("Invalid Movement mode!"));
	}
}

void UCocaineMovementComponent::EnterSlide()
{
	bWantsToCrouch=true;
	Velocity+=Velocity.GetSafeNormal2D()*Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom,CMOVE_Slide);
}

void UCocaineMovementComponent::ExitSlide()
{
	bWantsToCrouch=false;
	FQuat NewRotation=FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(),FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector,NewRotation,true,Hit);
	SetMovementMode(MOVE_Walking);
}

void UCocaineMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime<MIN_TICK_TIME)
	{
		return;
	}
	RestorePreAdditiveRootMotionVelocity();
	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit)||Velocity.SizeSquared()<pow(Slide_MinSpeed,2))
	{
		ExitSlide();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	//Surface Gravity
	Velocity+=Slide_GravityForce*FVector::DownVector*DeltaTime;

	//Strafe
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(),UpdatedComponent->GetRightVector()))>.5)
	{
		Acceleration=Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration=FVector::ZeroVector;
	}
	//Calc Velocity
	if (!HasAnimRootMotion()&&!CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime,Slide_Friction,true,GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(DeltaTime);

	//Perform Move
	Iterations++;
	bJustTeleported=false;

	FVector OldLocation=UpdatedComponent->GetComponentLocation();
	FQuat OldRotation=UpdatedComponent->GetComponentQuat();
	FHitResult Hit(1.f);
	FVector Adjusted=Velocity*DeltaTime;
	FVector VelPlaneDir=FVector::VectorPlaneProject(Velocity,SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation=FRotationMatrix::MakeFromXZ(VelPlaneDir,SurfaceHit.Normal).ToQuat();
	SafeMoveUpdatedComponent(Adjusted,NewRotation,true,Hit);

	if (Hit.Time<1.f)
	{
		HandleImpact(Hit,DeltaTime,Adjusted);
		SlideAlongSurface(Adjusted,(1.f-Hit.Time),Hit.Normal,Hit,true);
	}

	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit)||Velocity.SizeSquared()<pow(Slide_MinSpeed,2))
	{
		ExitSlide();
	}

	//Update Outgoing Velocity & Acceleration
	if (!bJustTeleported && !HasAnimRootMotion()&&!CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity=(UpdatedComponent->GetComponentLocation()-OldLocation)/DeltaTime;
	}
}

bool UCocaineMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start=UpdatedComponent->GetComponentLocation();
	FVector End=Start+CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2.f*FVector::DownVector;
	FName ProfileName=TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit,Start,End,ProfileName,CocaineCharacterOwner->GetIgnoreCharacterParams());
}
#pragma region Input
//toggles flag
void UCocaineMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}
//same as the other but with a twist
void UCocaineMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}
//Togless crouching
void UCocaineMovementComponent::CrouchPressed()
{
	bWantsToCrouch=!bWantsToCrouch;
}

bool UCocaineMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode==MOVE_Custom&&CustomMovementMode==InCustomMovementMode;
}
#pragma endregion
UCocaineMovementComponent::UCocaineMovementComponent()
{
	NavAgentProps.bCanCrouch=true;
}

