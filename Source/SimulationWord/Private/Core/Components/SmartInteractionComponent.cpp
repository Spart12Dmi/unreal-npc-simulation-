// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationWord/Public/Core/Components/SmartInteractionComponent.h"

#include "Core/Subsystems/SmartInteractionSubsystem.h"

USmartInteractionComponent::USmartInteractionComponent()
{
	bWantsInitializeComponent = true;
}

void USmartInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();
	RegisterObject();
}

bool USmartInteractionComponent::IsComponentAccessible() const
{
	return Workers.Num() < MaxWorkers;
}

void USmartInteractionComponent::ReserveSmartObject( ABaseNPC* BaseNPC) 
{
	Workers.AddUnique(BaseNPC);
}

void USmartInteractionComponent::ReleaseSmartObject(ABaseNPC* BaseNPC)
{
	Workers.Remove(BaseNPC);
}

TArray<TObjectPtr<UBasedInteractionTask>> USmartInteractionComponent::GetInteractionTasks() const
{
	return InteractionTasks;
}

void USmartInteractionComponent::RegisterObject()
{
	// register object in world
	if (USmartInteractionSubsystem* Subsystem =
		UWorld::GetSubsystem<USmartInteractionSubsystem>(GetWorld()))
	{
		Subsystem->InitializeNewSmartObject(this);
	}
}
