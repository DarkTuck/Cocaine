// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CocaineEnemyManager.generated.h"

class AShooterNPC;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNPCsAllDead,int,id);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COCAINE_API UCocaineEnemyManager : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FNPCsAllDead OnAllNPCsDead;
private:
	int ID;
	TArray<AShooterNPC*> NPCs;

public:
	// Sets default values for this component's properties
	UCocaineEnemyManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	virtual void SetId(int id);
	UFUNCTION(BlueprintCallable)
	virtual void AddNPC(AShooterNPC* NPC);
	UFUNCTION(BlueprintCallable)
	virtual void RemoveNPC(AShooterNPC* NPC);
};
