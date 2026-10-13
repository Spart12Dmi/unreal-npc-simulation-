// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationWord/Public/Core/Subsystems/SmartInteractionSubsystem.h"

#include "Core/Components/SmartInteractionComponent.h"

void USmartInteractionSubsystem::InitializeNewSmartObject(USmartInteractionComponent* SmartObjectComponent)
{
	if (!IsValid(SmartObjectComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Provide an invalid USmartInteractionComponent"));
		return;
	}
	//check whether list of that smart object type exists in level
	TArray<TWeakObjectPtr<USmartInteractionComponent>>& RelevantActors =
						SmartObjectsMap.FindOrAdd(SmartObjectComponent->GetGameplayTag());
	//add new smart object to the level
	RelevantActors.Add(SmartObjectComponent);
}

TWeakObjectPtr<USmartInteractionComponent> USmartInteractionSubsystem::FindAppropriateSmartObject(
	const FGameplayTag Tag) 
{
	if (TArray<TWeakObjectPtr<USmartInteractionComponent>>* RelevantActors = SmartObjectsMap.Find(Tag))
	{
		for (const auto& WeakComponent : *RelevantActors)
		{
			if ( USmartInteractionComponent* Component = WeakComponent.Get(); 
				IsValid(Component) && Component->IsComponentAccessible())
			{
				return Component;
			}
		}
	}
	
	return nullptr;
}
