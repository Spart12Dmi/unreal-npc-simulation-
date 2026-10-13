// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/ScheduleTemplateDataAsset.h"

FGameplayTag UScheduleTemplateDataAsset::GetActivationTagAtTime(const FTimespan Time) const
{
	if (ScheduleActivities.IsEmpty())
	{
		return FGameplayTag::EmptyTag;
	}

	const FTimespan CurrentTime(Time.GetHours(), Time.GetMinutes(), Time.GetSeconds());

	// Before today's first entry, continue yesterday's last activity.
	FGameplayTag ActivationTag = ScheduleActivities.Last().ScheduleTag;

	// Schedule entries must be sorted by start time in ascending order.
	for (const FScheduleActivityRow& ScheduleActivity : ScheduleActivities)
	{
		if (ScheduleActivity.GetStartTime() > CurrentTime)
		{
			break;
		}
		//saved the latest activity which was before time of the next activity  
		ActivationTag = ScheduleActivity.ScheduleTag;
	}
	return ActivationTag;
}
