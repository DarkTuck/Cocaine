// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/CocaineMovementComponent.h"

#include "GameFramework/Character.h"

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
	UCocaineMovementComponent* CharacterMovement=Cast<UCocaineMovementComponent>(C->GetCharacterMovement());
	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
}

void UCocaineMovementComponent::FSavedMove_Cocaine::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	UCocaineMovementComponent* CharacterMovement=Cast<UCocaineMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
}

UCocaineMovementComponent::FNetworkPredictionData_Client_Cocaine::FNetworkPredictionData_Client_Cocaine(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr UCocaineMovementComponent::FNetworkPredictionData_Client_Cocaine::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Cocaine());
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
}

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

UCocaineMovementComponent::UCocaineMovementComponent()
{
	
}

