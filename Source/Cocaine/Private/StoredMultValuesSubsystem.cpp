// Fill out your copyright notice in the Description page of Project Settings.


#include "StoredMultValuesSubsystem.h"

#include "CocaineGameMode.h"

UStoredMultValuesSubsystem::UStoredMultValuesSubsystem() : SlideMultValue(DefaultMultValue), JumpMultValue(DefaultMultValue), KickMultValue(DefaultMultValue), 
DashMultValue(DefaultMultValue), GrindMultValue(DefaultMultValue), MantleMultValue(DefaultMultValue), KillMultValue(DefaultMultValue), GrappleMultValue(DefaultMultValue),
HeadshotMultValue(1), 
Mults(TMap<EMultType, int*>{{Slide,&SlideMultValue},
		{Jump,&JumpMultValue},
		{Kick,&KickMultValue},
		{KickedEnemy,&KickedEnemyMultValue},
		{Dash,&DashMultValue},
		{Grind,&GrindMultValue},
		{Mantle,&MantleMultValue},
		{Kill,&KillMultValue},
		{Grapple,&GrappleMultValue},
		{Headshot,&HeadshotMultValue}})
{
}

int UStoredMultValuesSubsystem::GetMultValue(const EMultType MultType) const
{
	return *Mults[MultType];
}

void UStoredMultValuesSubsystem::SetMultValue(const EMultType MultType, const int NewMultValue) const
{
	*Mults[MultType]=NewMultValue;
}
