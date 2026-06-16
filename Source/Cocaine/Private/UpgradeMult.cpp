// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeMult.h"

#include "CocaineCashGameInstanceSubsystem.h"
#include "StoredMultValuesSubsystem.h"
#include "Kismet/GameplayStatics.h"

int UUpgradeMult::GetCost() const
{
	return Cost;
}

void UUpgradeMult::ApplyUpgrade()
{
	UCocaineCashGameInstanceSubsystem* GI = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCocaineCashGameInstanceSubsystem>();
	const UStoredMultValuesSubsystem* SM = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UStoredMultValuesSubsystem>();
	if (GI->GetCash()>=Cost)
	{
		GI->AddCash(-Cost);
		SM->SetMultValue(Type,Mult);
	}
	
}
