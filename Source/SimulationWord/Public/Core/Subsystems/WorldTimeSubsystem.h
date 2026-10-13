// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldTimeSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONWORD_API UWorldTimeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	FString GetClockString() const;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UWorldTimeSubsystem, STATGROUP_Tickables);
	}
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	FTimespan GetWorldTime() const;
	
private:
	FTimespan WorldTime{ FTimespan::FromHours(8.0f) };
	
	double TimeScale{10.};
};
