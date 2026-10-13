// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Subsystems/WorldTimeSubsystem.h"

#include "Core/Settings/SimulationWorldSettings.h"


void UWorldTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TimeScale = GetDefault<USimulationWorldSettings>()->TimeScale;
}

FTimespan UWorldTimeSubsystem::GetWorldTime() const
{
	return WorldTime;
}

FString UWorldTimeSubsystem::GetClockString() const
{
	//to write hour:seconds and a m or p m
	constexpr int32 NoonHour = 12;
	int32 Hour = WorldTime.GetHours();
	const bool bIsAM = Hour < NoonHour;
	Hour = Hour % NoonHour;
	const FString OutTime = FString::Printf(TEXT("%02d:%02d "), Hour, WorldTime.GetMinutes());
	const TCHAR* Period = bIsAM ? TEXT("AM") : TEXT("PM");
	return OutTime + Period;
}

void UWorldTimeSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//update world time
	WorldTime += FTimespan::FromSeconds(DeltaTime*TimeScale);
}
