// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BehaviorTree_Tasks/BTTask_ExtractExecutionTasks.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "NPCS/BaseAiController.h"
#include "NPCS/Blackboard/ExecutionTasksData.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"
#include "NPCS/Tasks/BasedInteractionTask.h"
#include "StructUtils/StructView.h"

UBTTask_ExtractExecutionTasks::UBTTask_ExtractExecutionTasks()
{
	NodeName = TEXT("Extract Smart Interaction Tasks");
}

EBTNodeResult::Type UBTTask_ExtractExecutionTasks::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	//save selected smartobjectcomponent to blackboard
	UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
	if ( !BBComponent )
	{
		UE_LOG(LogTemp, Error, TEXT("BBComponent is NULL"));
		return EBTNodeResult::Failed;
	}
	//get saved smart object from blackboard
	const USmartInteractionComponent* InteractionComponent = 
		Cast<USmartInteractionComponent>(BBComponent->GetValueAsObject(UNPCBlackboardData::SelectedInteractionKey));
	if ( !InteractionComponent )
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionComponent is NULL"));
		//reset tasks if no smart object exists
		const FExecutionTasksData EmptyTasks;
		BBComponent->SetValue<UBlackboardKeyType_Struct>(UNPCBlackboardData::ExecutionTasksKey,
			FConstStructView::Make(EmptyTasks));
		return EBTNodeResult::Failed;
	}
	const TArray<TObjectPtr<UBasedInteractionTask>> SourceTasks = InteractionComponent->GetInteractionTasks();
	TArray<TObjectPtr<UBasedInteractionTask>> ExecutionTasks;
	ExecutionTasks.Reserve(SourceTasks.Num());

	// Copying the array would still reference the component's task objects.
	// Duplicate each task so every interaction has its own pawn and execution state.
	for (const TObjectPtr<UBasedInteractionTask>& SourceTask : SourceTasks)
	{
		if (!IsValid(SourceTask.Get()))
		{
			UE_LOG(LogTemp, Error, TEXT("Interaction contains an invalid task"));
			return EBTNodeResult::Failed;
		}

		ExecutionTasks.Add(DuplicateObject<UBasedInteractionTask>(SourceTask.Get(), BBComponent));
	}

	//save extracted tasks into blackboard
	const FExecutionTasksData TasksData(MoveTemp(ExecutionTasks));
	const bool bStored = BBComponent->SetValue<UBlackboardKeyType_Struct>( UNPCBlackboardData::ExecutionTasksKey,
											FConstStructView::Make(TasksData));
	if (!bStored)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionTasks not stored"));
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Succeeded;
}
