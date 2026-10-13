// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ScheduleTemplateDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FScheduleActivityRow
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "23"))
	int32 StartHour = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "59"))
	int32 StartMinute = 0;

	FTimespan GetStartTime() const
	{
		return FTimespan(StartHour, StartMinute, 0);
	}

	float GetActivationTime() const
	{
		return StartHour + static_cast<float>(StartMinute)/60.f;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag ScheduleTag;
};

/**
 * scheduler for the npcs which defines what NPC does during the day
 */
UCLASS(BlueprintType, Blueprintable)
class SIMULATIONWORD_API UScheduleTemplateDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FGameplayTag GetActivationTagAtTime(const FTimespan Time) const;
	
private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	TArray<FScheduleActivityRow> ScheduleActivities;
	
	
};
