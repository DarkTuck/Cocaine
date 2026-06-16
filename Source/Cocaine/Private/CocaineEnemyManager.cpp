// Fill out your copyright notice in the Description page of Project Settings.


#include "CocaineEnemyManager.h"

#include "ShooterNPC.h"


// Sets default values for this component's properties
UCocaineEnemyManager::UCocaineEnemyManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCocaineEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCocaineEnemyManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCocaineEnemyManager::SetId(int id)
{
	ID = id;
}

void UCocaineEnemyManager::AddNPC(AShooterNPC* NPC)
{
	NPCs.Add(NPC);
	NPC->CocaineEnemyManager=this;
}

void UCocaineEnemyManager::RemoveNPC(AShooterNPC* NPC)
{
	NPCs.Remove(NPC);
	if (NPCs.Num() == 0)
	{
		OnAllNPCsDead.Broadcast(ID);
	}
}

