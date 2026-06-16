// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBuyableUpgrade.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIBuyableUpgrade : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COCAINE_API IIBuyableUpgrade
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int GetCost() const = 0;
	virtual void ApplyUpgrade(); 
};
