// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BehaviorTree_Tasks/BTTask_ReserveSmartInteraction.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "Core/Subsystems/SmartInteractionSubsystem.h"
#include "Core/Subsystems/WorldTimeSubsystem.h"
#include "DataAssets/ScheduleTemplateDataAsset.h"
#include "NPCS/BaseAiController.h"
#include "NPCS/Blackboard/ExecutionTasksData.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"
#include "StructUtils/StructView.h"

UBTTask_ReserveSmartInteraction::UBTTask_ReserveSmartInteraction()
{
	NodeName = TEXT("Reserve Smart Interaction");
}

void UBTTask_ReserveSmartInteraction::ReleasePreviousInteractionComponent(UBlackboardComponent*& BBComponent, 
																							ABaseNPC*& NPCOwner)
{
	USmartInteractionComponent* PreviousInteraction = Cast<USmartInteractionComponent>(
		BBComponent->GetValueAsObject(UNPCBlackboardData::SelectedInteractionKey));
	if (IsValid(PreviousInteraction))
	{
		PreviousInteraction->ReleaseSmartObject(NPCOwner);
	}
	BBComponent->ClearValue(UNPCBlackboardData::SelectedInteractionKey);
}

EBTNodeResult::Type UBTTask_ReserveSmartInteraction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAiController* AIController = Cast<ABaseAiController>(OwnerComp.GetAIOwner());
	if(!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController is NULL"));
		return EBTNodeResult::Failed;
	}
	ABaseNPC* NPCOwner = AIController->GetNPCOwner();
	if(!NPCOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController is NULL"));
		return EBTNodeResult::Failed;
	}
	const UScheduleTemplateDataAsset* Schedule = NPCOwner->GetNPCSchedule();
	if(!Schedule)
	{
		UE_LOG(LogTemp, Error, TEXT("Schedule is NULL"));
		return EBTNodeResult::Failed;
	}
	
	//get current world time
	const UWorld* World = GetWorld();
	UWorldTimeSubsystem* WorldTimeSubsystem = World ? World->GetSubsystem<UWorldTimeSubsystem>() : nullptr;
	if ( !WorldTimeSubsystem )
	{
		UE_LOG(LogTemp, Error, TEXT("World time subsystem is NULL"));
		return EBTNodeResult::Failed;
	}
	FTimespan CurrentWorldTime = WorldTimeSubsystem->GetWorldTime();
	//get smart tag by schedule which object npc should select
	FGameplayTag SmartObjectTag = Schedule->GetActivationTagAtTime(CurrentWorldTime);
	//init blackboard
	UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
	if ( !BBComponent )
	{
		UE_LOG(LogTemp, Error, TEXT("BBComponent is NULL"));
		return EBTNodeResult::Failed;
	}
	USmartInteractionSubsystem* SmartInteractionSubsystem = World->GetSubsystem<USmartInteractionSubsystem>();
	if ( !SmartInteractionSubsystem )
	{
		UE_LOG(LogTemp, Error, TEXT("World time subsystem is NULL"));
		return EBTNodeResult::Failed;
	}
	
	// Release before searching so the previous component's slot is available again.
	ReleasePreviousInteractionComponent(BBComponent, NPCOwner);
	// UE 5.7's ClearValue skips instanced struct keys because IsEmpty sees their zero-sized buffer.
	const FExecutionTasksData EmptyTasks;
	BBComponent->SetValue<UBlackboardKeyType_Struct>(UNPCBlackboardData::ExecutionTasksKey,
		FConstStructView::Make(EmptyTasks));
	BBComponent->SetValueAsInt(UNPCBlackboardData::CurrentTaskIndexKey, 0);

	const TWeakObjectPtr<USmartInteractionComponent> SmartObject = 
		SmartInteractionSubsystem->FindAppropriateSmartObject(SmartObjectTag);
	USmartInteractionComponent* SmartObjectComponent = SmartObject.Get();
	if ( !IsValid(SmartObjectComponent) )
	{
		UE_LOG(LogTemp, Error, TEXT("No appropriate smart object in world"));
		return EBTNodeResult::Failed;
	}
	//assign owner npc to this smart object ptr
	SmartObjectComponent->ReserveSmartObject(NPCOwner);
	//save selected smartobjectcomponent to blackboard
	BBComponent->SetValueAsObject(UNPCBlackboardData::SelectedInteractionKey, SmartObjectComponent);
	return EBTNodeResult::Succeeded;
}

