// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IBuyableUpgrade.h"
#include "UObject/Object.h"
#include "UpgradeMult.generated.h"

enum EMultType : int;
/**
 * 
 */
UCLASS()
class COCAINE_API UUpgradeMult : public UObject, public IIBuyableUpgrade
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Upgrade")
	int Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Upgrade")
	int Mult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Upgrade")
	TEnumAsByte<EMultType> Type;
public:
	virtual int GetCost() const override;
	virtual void ApplyUpgrade() override;
};
