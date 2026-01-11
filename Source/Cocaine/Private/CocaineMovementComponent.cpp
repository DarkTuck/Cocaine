// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/CocaineMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "GrindingRail.h"

// Helper Macros
#if 1
float MacroDuration = 2.f;
#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, MacroDuration ? MacroDuration:-1.f, FColor::Yellow,x); //write debug on screen macro
#define POINT(x,c) DrawDebugPoint(GetWorld(),x,10,c,!MacroDuration,MacroDuration); // draw debug point macro
#define LINE(x1,x2,c) DrawDebugLine(GetWorld(),x1,x2,c,!MacroDuration,MacroDuration); // draw debug line macro
#define CAPSULE(x,c) DrawDebugCapsule(GetWorld(),x,CapHH(),CapR(),FQuat::Identity,c,!MacroDuration,MacroDuration); // draw debug capsule macro
#define SPHERE(c,r,color) DrawDebugSphere(GetWorld(),c,r,32,color,false,MacroDuration); // draw debug sphere macro
#else
#define SLOG(x)
#define POINT(x,c)
#define LINE(x,c)
#define CAPSULE(x,c)
#define SPHERE(c,r,color)
#endif

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
	if (Saved_bWantsToDash!=NewCocaineMove->Saved_bWantsToDash)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UCocaineMovementComponent::FSavedMove_Cocaine::Clear()
{
	FSavedMove_Character::Clear();
	
	Saved_bWantsToSprint = 0;
	Saved_bWantsToDash = 0;
	Saved_bPressedCocaineJump=0;
	
	Saved_bHadAnimRootMotion=0;
	Saved_bTransitionFinished=0;
	
	Saved_bWantsToProne = 0;
	Saved_bPrevWantsToCrouch = 0;
}

uint8 UCocaineMovementComponent::FSavedMove_Cocaine::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (Saved_bWantsToSprint) Result |=  FLAG_Sprint;
	if (Saved_bWantsToDash) Result |=  FLAG_Dash;
	if (Saved_bPressedCocaineJump) Result |= FLAG_JumpPressed;
	
	return Result;
}

void UCocaineMovementComponent::FSavedMove_Cocaine::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	const UCocaineMovementComponent* CharacterMovement=Cast<UCocaineMovementComponent>(C->GetCharacterMovement());
	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch=CharacterMovement->Safe_bPrevWantsToCrouch;
	Saved_bWantsToProne=CharacterMovement->Safe_bWantsToProne;
	Saved_bWantsToDash=CharacterMovement->Safe_bWantsToDash;
	
	Saved_bPressedCocaineJump=CharacterMovement->CocaineCharacterOwner->bPressedCocaineJump;
	
	Saved_bHadAnimRootMotion = CharacterMovement->Safe_bHadAnimRootMotion;
	Saved_bTransitionFinished = CharacterMovement->Safe_bTransitionFinished;
}

void UCocaineMovementComponent::FSavedMove_Cocaine::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	UCocaineMovementComponent* CharacterMovement=Cast<UCocaineMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch=Saved_bPrevWantsToCrouch;
	CharacterMovement->Safe_bWantsToProne=Saved_bWantsToProne;
	CharacterMovement->Safe_bWantsToDash=Saved_bWantsToDash;
	
	CharacterMovement->CocaineCharacterOwner->bPressedCocaineJump=Saved_bPressedCocaineJump;
	CharacterMovement->Safe_bHadAnimRootMotion=Saved_bHadAnimRootMotion;
	CharacterMovement->Safe_bTransitionFinished=Saved_bTransitionFinished;
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

#pragma region CMC
UCocaineMovementComponent::UCocaineMovementComponent() 
{
	NavAgentProps.bCanCrouch=true;
}
void UCocaineMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CocaineCharacterOwner=Cast<ACocaineCharacter>(GetOwner());
}
// Network
void UCocaineMovementComponent::UpdateFromCompressedFlags(uint8 InFlags)
{
	Super::UpdateFromCompressedFlags(InFlags);

	Safe_bWantsToSprint =(InFlags & FSavedMove_Cocaine::FLAG_Sprint)!=0;
	Safe_bWantsToDash =(InFlags & FSavedMove_Cocaine::FLAG_Dash)!=0;
}
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

// Getters / Helpers
bool UCocaineMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround()||IsCustomMovementMode(CMOVE_Slide)||IsCustomMovementMode(CMOVE_Prone);
}
bool UCocaineMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState()&&IsMovingOnGround();
}
float UCocaineMovementComponent::GetMaxSpeed() const
{
	if (IsMovementMode(MOVE_Walking)&&Safe_bWantsToSprint && !IsCrouching()) return MaxSprintSpeed;
	
	if (MovementMode!=MOVE_Custom) return Super::GetMaxSpeed();

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return MaxSlideSpeed;
	case CMOVE_Prone:
		return ProneMaxSpeed;
	case CMOVE_Mantle:
		return MaxSprintSpeed;
	default:
;		UE_LOG(LogTemp,Fatal,TEXT("Invalid Movement Mode"));
		return -1.f;
	}
}
float UCocaineMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode!=MOVE_Custom) return Super::GetMaxBrakingDeceleration();
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return BrakingDecelerationSliding;
	case CMOVE_Prone:
		return BrakingDecelerationProning;
	default:
		UE_LOG(LogTemp,Warning,TEXT("Invalid Movement Mode"));
		return -1.f;
	}
}

// Movement Pipeline
void UCocaineMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Slide
	if (MovementMode==MOVE_Walking&&!bWantsToCrouch&&Safe_bPrevWantsToCrouch)
	{
		if (CanSlide())
		{
			SetMovementMode(MOVE_Custom, CMOVE_Slide);
		}
	}// Enter
	if (IsCustomMovementMode(CMOVE_Slide)&&!bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}
	
	// Prone
	if (Safe_bWantsToProne)
	{
		if (CanProne())
		{
			SetMovementMode(MOVE_Custom, CMOVE_Prone);
			if (!CharacterOwner->HasAuthority()) Server_EnterProne();
		}
		Safe_bWantsToProne=false;
	}// Enter
	if (IsCustomMovementMode(CMOVE_Prone)&&!bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}

	// Dash
	const bool bAuthProxy = CharacterOwner->HasAuthority() && !CharacterOwner->IsLocallyControlled();
	if (Safe_bWantsToDash&&CanDash())
	{
		if (!bAuthProxy||GetWorld()->GetTimeSeconds() - DashStartTime > AuthDashCooldownDuration)
		{
			bRootMotionDash?PerformDashRootMotion():PerformDash();
			Safe_bWantsToDash=false;
			Proxy_bDashStart=!Proxy_bDashStart;
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Client tried to cheat"));
		}
	}
	
	// Try Mantle
	if (CocaineCharacterOwner->bPressedCocaineJump)
	{
		if (TryMantle())
		{
			CocaineCharacterOwner->StopJumping();
			SetMovementMode(MOVE_Custom,CMOVE_Mantle);
		}
		else
		{
			SLOG("Failed Mantle, Reverting to jump")
			CocaineCharacterOwner->bPressedCocaineJump = false;
			CharacterOwner->bPressedJump=true;
			CharacterOwner->CheckJumpInput(DeltaSeconds);
		}
	}
		
	// Transition Mantle
	if (Safe_bTransitionFinished)
	{
		SLOG("Transition Finished")
		UE_LOG(LogTemp,Warning,TEXT("Finished RM"));
		if (IsValid(TransitionQueuedMontage))
		{
			SetMovementMode(MOVE_Flying);
			CharacterOwner->PlayAnimMontage(TransitionQueuedMontage, TransitionQueuedMontageSpeed);
			TransitionQueuedMontageSpeed=0.f;
			TransitionQueuedMontage=nullptr;
		}
		else
		{
			SetMovementMode(MOVE_Walking);
		}
		Safe_bTransitionFinished=false;
	}
	
	// Grinding
	if (TryGrind())
	{
		// TODO
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UCocaineMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
	
	if (!HasAnimRootMotion()&&Safe_bHadAnimRootMotion&&IsMovementMode(MOVE_Flying))
	{
		UE_LOG(LogTemp,Warning,TEXT("Ending Anim Root Motion"));
		SetMovementMode(MOVE_Walking);
	}
	if (GetRootMotionSourceByID(TransitionRMS_ID)&&GetRootMotionSourceByID(TransitionRMS_ID)->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		RemoveRootMotionSourceByID(TransitionRMS_ID);
		Safe_bTransitionFinished=true;
	}
	
	Safe_bHadAnimRootMotion=HasAnimRootMotion();
}

void UCocaineMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (IsMovementMode(MOVE_Flying) && !HasRootMotionSources()) SetMovementMode(MOVE_Walking);
	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}
void UCocaineMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	case CMOVE_Prone:
		PhysProne(deltaTime, Iterations);
		break;
	case CMOVE_Mantle:
		PhysMantle(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp,Fatal,TEXT("Invalid Movement mode!"));
	}
}

// Movement Event
void UCocaineMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
 {
 	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
 	if (PreviousMovementMode==MOVE_Custom && PreviousCustomMode==CMOVE_Slide) ExitSlide();
 	if (PreviousMovementMode==MOVE_Custom && PreviousMovementMode==CMOVE_Prone) ExitProne();
 	
 	if (IsCustomMovementMode(CMOVE_Slide)) EnterSlide(PreviousMovementMode, (ECustomMovementMode)PreviousMovementMode);
 	if (IsCustomMovementMode(CMOVE_Prone)) EnterProne(PreviousMovementMode, (ECustomMovementMode)PreviousMovementMode);
 }
#pragma endregion

#pragma region Slide
void UCocaineMovementComponent::EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	bWantsToCrouch = true;
	bOrientRotationToMovement = false;
	Velocity += Velocity.GetSafeNormal2D() * SlideEnterImpulse;

	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
}

void UCocaineMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
	bOrientRotationToMovement = true;
}

bool UCocaineMovementComponent::CanSlide() const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, CocaineCharacterOwner->GetIgnoreCharacterParams());
	bool bEnoughSpeed = Velocity.SizeSquared() > pow(MinSlideSpeed, 2);
	
	return bValidSurface && bEnoughSpeed;
}

void UCocaineMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	
	if (!CanSlide())
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = DeltaTime;

	// Perform the move
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent * const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0.f;
		Velocity += SlopeForce * SlideGravityForce * DeltaTime;
		
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector().GetSafeNormal2D());

		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction * SlideFrictionFactor, false, GetMaxBrakingDeceleration());
		
		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		bool bFloorWalkable = CurrentFloor.IsWalkableFloor();

		if ( bZeroDelta )
		{
			remainingTime = 0.f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			if ( IsFalling() )
			{
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f,ActualDist/DesiredDist));
				}
				StartNewPhysics(remainingTime,Iterations);
				return;
			}
			else if ( IsSwimming() ) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}


		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if ( bCheckLedges && !CurrentFloor.IsWalkableFloor() )
		{
			// calculate possible alternate movement
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, OldFloor);
			if ( !NewDelta.IsZero() )
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ( (bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if ( IsSwimming() )
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;
			}
		}
		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround() && bFloorWalkable)
		{
			// Make velocity reflect actual move
			if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}


	FHitResult Hit;
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(Velocity.GetSafeNormal2D(), FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, Hit);
			
}

bool UCocaineMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	const FVector Start=UpdatedComponent->GetComponentLocation();
	const FVector End=Start+CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2.f*FVector::DownVector;
	const FName ProfileName=TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit,Start,End,ProfileName,CocaineCharacterOwner->GetIgnoreCharacterParams());
}
#pragma endregion

#pragma region Prone
void UCocaineMovementComponent::Server_EnterProne_Implementation()
{
	Safe_bWantsToProne=true;
}

void UCocaineMovementComponent::EnterProne(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	bWantsToCrouch=true;
	
	if (PrevMode==MOVE_Custom&&PrevCustomMode==CMOVE_Slide)
	{
		Velocity+=Velocity.GetSafeNormal2D()* ProneSlideEnterImpulse;
	}
	
	FindFloor(UpdatedComponent->GetComponentLocation(),CurrentFloor,true,NULL);
}

void UCocaineMovementComponent::ExitProne()
{
}

bool UCocaineMovementComponent::CanProne() const
{
	return IsCustomMovementMode(CMOVE_Slide)||(IsMovementMode(MOVE_Walking)&&IsCrouching());
}

//a lot of this function is borrowed from UCharacterMovementComponent::PhysWalking
void UCocaineMovementComponent::PhysProne(float DeltaTime, int32 Iterations)
{
	if (DeltaTime<MIN_TICK_TIME)
	{
		return;
	}
	
	if (!CharacterOwner||(!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion()&&!CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration=FVector::ZeroVector;
		Velocity=FVector::ZeroVector;
		return;
	}
	
	bJustTeleported=false;
	bool bCheckedFall=false;
	bool bTriedLedgeMove=false;
	float remainingTime=DeltaTime;
	
	//perform move
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported=false;
		const float timeTick = GetSimulationTimeStep(remainingTime,Iterations);
		remainingTime-=timeTick;
		
		//Save current values
		UPrimitiveComponent * const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation=(OldBase!=NULL)?OldBase->GetComponentLocation():FVector(0,0,0);
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;
		
		//Ensure velocity is horizontal
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;
		Acceleration.Z=0.f;
		
		//Apply acceleration
		CalcVelocity(timeTick,GroundFriction,false,GetMaxBrakingDeceleration());
		
		//Compute new parameters
		const FVector MoveVelocity=Velocity;
		const FVector Delta=timeTick*MoveVelocity;//dx=v*dt
		const bool bZeroDelta=Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		
		if (bZeroDelta)
		{
			remainingTime=0.f;
		}
		else
		{
			MoveAlongFloor(MoveVelocity,timeTick,&StepDownResult);
			
			if (IsFalling())
			{
				// pawn decided to jump
				const float DesiredDist=Delta.Size();
				if (DesiredDist>KINDA_SMALL_NUMBER)
				{
					const float ActualDist=(UpdatedComponent->GetComponentLocation()-OldLocation).Size2D();
					remainingTime+=timeTick*(1.f-FMath::Min(1.f,ActualDist/DesiredDist));
				}
				StartNewPhysics(remainingTime,Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation,OldVelocity,timeTick,remainingTime,Iterations);
				return;
			}
		}
		
		// Update floor
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor=StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(),CurrentFloor,bZeroDelta,NULL);
		}
		
		//check for ledges here
		const bool bCheckLedges=!CanWalkOffLedges();
		if (bCheckLedges&&!CurrentFloor.IsWalkableFloor())
		{
			//Calculate possible alternate movement
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, OldFloor);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta/timeTick;
				remainingTime += timeTick;
				Iterations--;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ( (bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBaseFromFloor(CurrentFloor);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + MAX_FLOOR_DIST * -GetGravityDirection();
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if ( IsSwimming() )
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;
			}
		}


		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}	
	}
	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}
#pragma endregion

#pragma region Dash

void UCocaineMovementComponent::OnDashCooldownFinished()
{
	Safe_bWantsToDash=true;
}

bool UCocaineMovementComponent::CanDash() const
{
	return IsWalking() && !IsCrouching() /*|| IsFalling()*/; //to uncomment if player could dash in the air (probably should be variable but for now dash can end up as unused code)
}

void UCocaineMovementComponent::PerformDash()
{
	DashStartTime=GetWorld()->GetTimeSeconds();
	
	FVector DashDirection = (Acceleration.IsNearlyZero() ? UpdatedComponent->GetForwardVector() : Acceleration).GetSafeNormal2D();
	DashDirection += FVector::UpVector * .1f;
	Velocity = DashImpulse * (DashDirection + FVector::UpVector * .1f);

	const FQuat NewRotation = FRotationMatrix::MakeFromXZ(DashDirection,FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector,NewRotation,false,Hit);
	
	SetMovementMode(MOVE_Falling);
	
	DashStartDelegate.Broadcast();
}

void UCocaineMovementComponent::PerformDashRootMotion()
{
	DashStartTime=GetWorld()->GetTimeSeconds();
	
	//changing mode to flying will not apply gravity to RootMotion animation (Z coordinate)
	SetMovementMode(bUseGravityInRootMotion ? MOVE_Falling : MOVE_Flying);
	CharacterOwner->PlayAnimMontage(DashMontage);
	
	DashStartDelegate.Broadcast();
}
#pragma endregion

#pragma region Mantle
bool UCocaineMovementComponent::TryMantle()
{
	if (!IsMovementMode(MOVE_Walking)&&!IsCrouching()&&!IsMovementMode(MOVE_Falling)) return false;

	// Helper Variables
	FVector BaseLocation = UpdatedComponent->GetComponentLocation()+FVector::DownVector*CapHH();
	FVector Fwd= UpdatedComponent->GetForwardVector().GetSafeNormal2D(); // Forward
	auto Params = CocaineCharacterOwner->GetIgnoreCharacterParams();
	float MaxHeight = CapHH()*2+MantleReachHeight;
	float CosMMWSA = FMath::Cos(FMath::DegreesToRadians(MantleMinWallSteepnessAngle)); // Cosine of MantleMinWallSteepnessAngle
	float CosMMSA = FMath::Cos(FMath::DegreesToRadians(MantleMaxSurfaceAngle)); // Cosine of MantleMaxSurfaceAngle
	float CosMMAA = FMath::Cos(FMath::DegreesToRadians(MantleMaxAlignmentAngle)); // Cosine of MantleMaxAlignmentAngle

	SLOG("TriedMantle");

	// Check Front Face
	FHitResult FrontHit;
	float CheckDistance = FMath::Clamp(Velocity|Fwd,CapR()+30,MantleMaxDistance);
	FVector FrontStart = BaseLocation+FVector::UpVector*(MaxStepHeight-1);
	int8 Iterations = 6; // interactions of line trace increasing it should help mantle to perform on thin geometry
	for (int8 i = 0; i < Iterations; i++)
	{
		LINE(FrontStart, FrontStart + Fwd * CheckDistance, FColor::Red);
		if (GetWorld()->LineTraceSingleByProfile(FrontHit, FrontStart, FrontStart + Fwd * CheckDistance, "BlockAll", Params)) break;
		FrontStart += FVector::UpVector * (2.f * CapHH() - (MaxStepHeight - 1)) / Iterations-1;
	}
	if (!FrontHit.IsValidBlockingHit())return false;
	float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	if (FMath::Abs(CosWallSteepnessAngle) > CosMMWSA || (Fwd | -FrontHit.Normal) < CosMMAA) return false;
	POINT(FrontHit.Location,FColor::Red);

	// Check Height
	TArray<FHitResult> HeightHits;
	FHitResult SurfaceHit;
	FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector,FrontHit.Normal).GetSafeNormal(); // vector that gets the tip of the wall helps with angled walls
	float WallCos = FVector::UpVector | FrontHit.Normal;
	float WallSin = FMath::Sqrt(1-WallCos*WallCos);
	FVector TraceStart =FrontHit.Location+Fwd+WallUp*(MaxHeight-(MaxStepHeight-1))/WallSin;
	LINE(TraceStart, FrontHit.Location + Fwd, FColor::Red);
	if (!GetWorld()->LineTraceMultiByProfile(HeightHits,TraceStart,FrontHit.Location+Fwd,"BlockAll", Params)) return false;
	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}
	if (!SurfaceHit.IsValidBlockingHit() || (SurfaceHit.Normal|FVector::UpVector)<CosMMSA) return false;
	float Height = (SurfaceHit.Location-BaseLocation) | FVector::UpVector;
	SLOG(FString::Printf(TEXT("Height %f"),Height));
	POINT(SurfaceHit.Location,FColor::Blue);

	if (Height>MaxHeight) return false;

	// Check Clearance
	float SurfaceCos = FVector::UpVector | SurfaceHit.Normal;
	float SurfaceSin = FMath::Sqrt(1-SurfaceCos*SurfaceCos);
	FVector ClearCapLocation = SurfaceHit.Location + Fwd * CapR() + FVector::UpVector*(CapHH() + 1 + CapR() * 2 * SurfaceSin);
	FCollisionShape CapShape = FCollisionShape::MakeCapsule(CapR(),CapHH());
	if (GetWorld()->OverlapAnyTestByProfile(ClearCapLocation,FQuat::Identity,"BlockAll",CapShape,Params))
	{
		CAPSULE(ClearCapLocation,FColor::Red);
		return false;
	}
	else
	{
		CAPSULE(ClearCapLocation,FColor::Green);
	}

	SLOG("Can Mantle")
	
	/*
	// Mantle Selection
	FVector ShortMantleTarget = GetMantleStartLocation(FrontHit,SurfaceHit,false);
	FVector TallMantleTarget = GetMantleStartLocation(FrontHit,SurfaceHit,true);
	
	bool bTallMantle = false;
	if (IsMovementMode(MOVE_Walking)&& Height > CapHH()*2)
	{
		bTallMantle = true;
	}
	else if (IsMovementMode(MOVE_Falling)&& (Velocity | FVector::UpVector)<0)
	{
		if (!GetWorld()->OverlapAnyTestByProfile(TallMantleTarget,FQuat::Identity,"BlockAll",CapShape,Params))
			bTallMantle = true;
	}
	FVector TransitionTarget = bTallMantle ? TallMantleTarget : ShortMantleTarget;
	CAPSULE(TransitionTarget,FColor::Yellow);
	// Perform Transition to Mantle
	CAPSULE(UpdatedComponent->GetComponentLocation(), FColor::Red)

	float UpSpeed = Velocity | FVector::UpVector;
	float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

	TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
	TransitionRMS.Reset();
	TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
	TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;
	
	TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .25f);
	SLOG(FString::Printf(TEXT("Duration: %f"), TransitionRMS->Duration))
		TransitionRMS->StartLocation = UpdatedComponent->GetComponentLocation();
	TransitionRMS->TargetLocation = TransitionTarget;

	// Apply Transition Root Motion Source
	Velocity = FVector::ZeroVector;
	SetMovementMode(MOVE_Flying);
	TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);

	// Animations
	if (TallMantleMontage&&ShortMantleMontage)
	{
		if (bTallMantle)
		{
			TransitionQueuedMontage = TallMantleMontage;
			CharacterOwner->PlayAnimMontage(TransitionTallMantleMontage, 1 / TransitionRMS->Duration);
			if (IsServer()) Proxy_bTallMantle = !Proxy_bTallMantle;
		}
		else
		{
			TransitionQueuedMontage = ShortMantleMontage;
			CharacterOwner->PlayAnimMontage(TransitionShortMantleMontage, 1 / TransitionRMS->Duration);
			if (IsServer()) Proxy_bShortMantle = !Proxy_bShortMantle;
		}
	}
	*/
	MantleTarget = ClearCapLocation;
	return true;
}

void UCocaineMovementComponent::EnterMantle(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
}

void UCocaineMovementComponent::ExitMantle()
{
	MantleTarget = FVector::ZeroVector;
	
}
//unfortunately, Mantle is not network safe, but it's unnecessary in this game. Might fix it later (maybe)

void UCocaineMovementComponent::PhysMantle(float DeltaTime, int32 Iterations)
{
	SLOG("PhysMantle")
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}
	
	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = DeltaTime;
	
		// Perform the move
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		FVector ToTarget = MantleTarget - OldLocation;
		const float Distance = ToTarget.Size();
	
		if (Distance < 10.f)
		{
			Velocity = FVector::ZeroVector;
			SetMovementMode(MOVE_Walking);
			StartNewPhysics(DeltaTime, Iterations);
			return;
		}
		ToTarget.Normalize();
		const float MaxSpeed = MantleMaxSpeed;
		const FVector DesiredVelocity = ToTarget * MaxSpeed;

		Velocity = FMath::VInterpTo(Velocity, DesiredVelocity, timeTick, 8.f);
 
		FVector Delta = Velocity * timeTick;
		if (Delta.IsNearlyZero())
		{
			remainingTime = 0.f;
			break;
		}
 
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
 
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit,false);
		}
 
		const FVector NewLocation = UpdatedComponent->GetComponentLocation();
		Velocity = (NewLocation - OldLocation) / timeTick;
 
		if (NewLocation.Equals(OldLocation, 0.001f))
		{
			remainingTime = 0.f;
			break; 
		}
 
		if (FVector::Dist(NewLocation, MantleTarget) < 10.f)
		{
			Velocity = FVector::ZeroVector;
			SetMovementMode(MOVE_Walking);
			remainingTime = 0.f;
			break;
		}
	}
}

FVector UCocaineMovementComponent::GetMantleStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit, const bool bTallMantle) const
{
	const float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	const float DownDistance = bTallMantle ? CapHH()*2.f : MaxStepHeight-1;
	const FVector EdgeTangent = FVector::CrossProduct(SurfaceHit.Normal,FrontHit.Normal).GetSafeNormal();
	FVector MantleStart = SurfaceHit.Location;
	MantleStart += FrontHit.Location.GetSafeNormal2D()*(2.f+CapR());
	MantleStart += UpdatedComponent->GetForwardVector().GetSafeNormal2D().ProjectOnTo(EdgeTangent)*CapR()*.3f;
	MantleStart += FVector::UpVector*CapHH();
	MantleStart += FVector::DownVector*DownDistance;
	MantleStart += FrontHit.Normal.GetSafeNormal2D()*CosWallSteepnessAngle*DownDistance;
	return MantleStart;
}
#pragma endregion

#pragma region Grind

bool UCocaineMovementComponent::TryGrind()
{
	if (!IsMovementMode(MOVE_Falling) || Velocity.Z>=0.0) return false;
	
	FHitResult Hit{};
	const FVector TraceStart {GetActorFeetLocation()};
	const FVector TraceEnd {TraceStart + FVector::DownVector};
	constexpr ECollisionChannel GrindCollisionChannel = ECC_GameTraceChannel3;
	GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity,GrindCollisionChannel,FCollisionShape::MakeSphere(GrindDetectionRadius));
	SPHERE(TraceStart,GrindDetectionRadius,FColor::Red);
	if (!Hit.bBlockingHit) return false;
	SLOG("Grind Hit")

	const AGrindingRail* GrindingRail = CastChecked<AGrindingRail>(Hit.GetActor());
	const FVector CharacterLocation = GetActorLocation();
	const USplineComponent* GrindSpline = GrindingRail->GetGrindRail();
	FTransform GrindSplineTransform = GrindSpline->FindTransformClosestToWorldLocation(CharacterLocation,ESplineCoordinateSpace::World);
	const FVector CharacterHeightOffset = GrindSplineTransform.GetUnitAxis(EAxis::Z)*CapHH();
	GrindSplineTransform.AddToTranslation(CharacterHeightOffset);
	
	if (FVector::Dist(GrindSplineTransform.GetLocation(),CharacterLocation) > GrindDetectionRadius) return false;
	
	SPHERE(GrindSplineTransform.GetLocation(),GrindDetectionRadius,FColor::Green);
	
	return true;
}
#pragma endregion 

#pragma region Helpers

bool UCocaineMovementComponent::IsServer() const
{
	return CharacterOwner->HasAuthority();
}

float UCocaineMovementComponent::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float UCocaineMovementComponent::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}
#pragma endregion

#pragma region Interface
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
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EnterProne,this,&UCocaineMovementComponent::TryEnterProne,Prone_EnterHoldDuration);
}
void UCocaineMovementComponent::CrouchReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EnterProne);
}

void UCocaineMovementComponent::DashPressed()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime-DashStartTime>=DashCooldownDuration)
	{
		Safe_bWantsToDash=true;
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_DashCooldown,this,&UCocaineMovementComponent::OnDashCooldownFinished,DashCooldownDuration-(CurrentTime-DashStartTime));
	}
}
void UCocaineMovementComponent::DashReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DashCooldown);
	Safe_bWantsToDash=false;
}

bool UCocaineMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode==MOVE_Custom&&CustomMovementMode==InCustomMovementMode;
}
bool UCocaineMovementComponent::IsMovementMode(EMovementMode InMovementMode) const
{
	return InMovementMode == MovementMode;
}
#pragma endregion

#pragma region Replication

void UCocaineMovementComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UCocaineMovementComponent,Proxy_bDashStart,COND_SkipOwner)
	
	DOREPLIFETIME_CONDITION(UCocaineMovementComponent, Proxy_bShortMantle,COND_SkipOwner)
	DOREPLIFETIME_CONDITION(UCocaineMovementComponent, Proxy_bTallMantle,COND_SkipOwner)
}

void UCocaineMovementComponent::OnRep_DashStart()
{
	if (Proxy_bDashStart)
	{
		if (bRootMotionDash) CharacterOwner->PlayAnimMontage(DashMontage);
		DashStartDelegate.Broadcast();
	}
}

void UCocaineMovementComponent::OnRep_ShortMantle()
{
	if (ProxyShortMantleMontage)
	{
		CharacterOwner->PlayAnimMontage(ProxyShortMantleMontage);
	}
}

void UCocaineMovementComponent::OnRep_TallMantle()
{
	if (ProxyTallMantleMontage)
	{
		CharacterOwner->PlayAnimMontage(ProxyTallMantleMontage);
	}
}
#pragma endregion