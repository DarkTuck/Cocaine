// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGenFunctions.h"

#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"

void URoomGenFunctions::GenerateNavMesh(FVector Location, FVector Extent, FRotator Rotation)
{
	UWorld* WorldContext = GEngine->GameViewport->GetWorld();
	
	ANavMeshBoundsVolume* NavMeshVolume = WorldContext->SpawnActor<ANavMeshBoundsVolume>(ANavMeshBoundsVolume::StaticClass(),Location,Rotation);
	if (!NavMeshVolume) return;
	NavMeshVolume->GetRootComponent()->Bounds = FBox(-Extent,Extent);

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(WorldContext))
	{
		NavSys->OnNavigationBoundsUpdated(NavMeshVolume);
		NavSys->Build();
	}
}
