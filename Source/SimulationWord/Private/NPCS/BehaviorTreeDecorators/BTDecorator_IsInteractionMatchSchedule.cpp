// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BehaviorTreeDecorators/BTDecorator_IsInteractionMatchSchedule.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "Core/Subsystems/WorldTimeSubsystem.h"
#include "DataAssets/ScheduleTemplateDataAsset.h"
#include "NPCS/BaseAiController.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"

UBTDecorator_IsInteractionMatchSchedule::UBTDecorator_IsInteractionMatchSchedule(
	const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Is Interaction matches Schedule");
}

bool UBTDecorator_IsInteractionMatchSchedule::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	ABaseAiController* AIController = Cast<ABaseAiController>(OwnerComp.GetAIOwner());
	if(!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController is NULL"));
		return false;
	}
	ABaseNPC* NPCOwner = AIController->GetNPCOwner();
	if(!NPCOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController is NULL"));
		return false;
	}
	const UScheduleTemplateDataAsset* Schedule = NPCOwner->GetNPCSchedule();
	if(!Schedule)
	{
		UE_LOG(LogTemp, Error, TEXT("Schedule is NULL"));
		return false;
	}
	
	//get current world time
	const UWorld* World = GetWorld();
	UWorldTimeSubsystem* WorldTimeSubsystem = World ? World->GetSubsystem<UWorldTimeSubsystem>() : nullptr;
	if ( !WorldTimeSubsystem )
	{
		UE_LOG(LogTemp, Error, TEXT("World time subsystem is NULL"));
		return false;
	}
	FTimespan CurrentWorldTime = WorldTimeSubsystem->GetWorldTime();
	//get smart tag by schedule which object npc should select
	FGameplayTag SmartObjectTag = Schedule->GetActivationTagAtTime(CurrentWorldTime);
	//init blackboard component
	UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
	if ( !BBComponent )
	{
		UE_LOG(LogTemp, Error, TEXT("BBComponent is NULL"));
		return false;
	}
	//achieve current executing component
	USmartInteractionComponent* SmartInteractionComponent = 
		Cast<USmartInteractionComponent>(BBComponent->GetValueAsObject(UNPCBlackboardData::SelectedInteractionKey));
	if ( !IsValid(SmartInteractionComponent) )
	{
		UE_LOG(LogTemp, Error, TEXT("SmartInteractionComponent is NULL"));
		return true;
	}
	const bool MatchesTag = SmartObjectTag == SmartInteractionComponent->GetGameplayTag();
	return !MatchesTag;
}
