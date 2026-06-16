// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StoredMultValuesSubsystem.generated.h"

enum EMultType : int;
/**
 * 
 */
UCLASS()
class COCAINE_API UStoredMultValuesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	UStoredMultValuesSubsystem();
private:
	int SlideMultValue;
	int JumpMultValue;
	int KickMultValue;
	int KickedEnemyMultValue;
	int DashMultValue;
	int GrindMultValue;
	int MantleMultValue;
	int KillMultValue;
	int GrappleMultValue;
	int HeadshotMultValue;
	
	const TMap<EMultType, int*> Mults;
	
	constexpr static int DefaultMultValue=20;

public:
	UFUNCTION(BlueprintCallable) int GetSavedSlideMultValue() const {return SlideMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedJumpMultValue() const {return JumpMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedKickMultValue() const {return KickMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedDashMultValue() const {return DashMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedGrindMultValue() const {return GrindMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedMantleMultValue() const {return MantleMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedKillMultValue() const {return KillMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedGrappleMultValue() const {return GrappleMultValue;};
	UFUNCTION(BlueprintCallable) int GetSavedHeadshotMultValue() const {return HeadshotMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedSlideMultValue(const int NewMultValue){SlideMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedJumpMultValue(const int NewMultValue){JumpMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedKickMultValue(const int NewMultValue){KickMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedDashMultValue(const int NewMultValue){DashMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedGrindMultValue(const int NewMultValue){GrindMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedMantleMultValue(const int NewMultValue){MantleMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedKillMultValue(const int NewMultValue){KillMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedGrappleMultValue(const int NewMultValue){GrappleMultValue=NewMultValue;};
	UFUNCTION(BlueprintCallable) void SetSavedHeadshotMultValue(const int NewMultValue){HeadshotMultValue=NewMultValue;};
	
	UFUNCTION(BlueprintCallable) int GetMultValue(const EMultType MultType) const;
	UFUNCTION(BlueprintCallable) void SetMultValue(const EMultType MultType, const int NewMultValue) const;
};
