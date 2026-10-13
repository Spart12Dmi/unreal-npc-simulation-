// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BehaviorTree_Tasks/BTTask_ExtractExecutionTasks.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "NPCS/BehaviorTreeHelpers.h"
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
	NPCBehaviorTree::FContext Context;
	if (!NPCBehaviorTree::TryGetContext(OwnerComp, Context))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComponent = Context.Blackboard;
	const USmartInteractionComponent* InteractionComponent =
		Cast<USmartInteractionComponent>(BBComponent->GetValueAsObject(UNPCBlackboardData::SelectedInteractionKey));
	if (!InteractionComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No smart interaction is selected"), *NodeName);
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
			UE_LOG(LogTemp, Error, TEXT("%s: Selected interaction contains an invalid task definition"), *NodeName);
			return EBTNodeResult::Failed;
		}

		ExecutionTasks.Add(DuplicateObject<UBasedInteractionTask>(SourceTask.Get(), BBComponent));
	}

	const FExecutionTasksData TasksData(MoveTemp(ExecutionTasks));
	const bool bStored = BBComponent->SetValue<UBlackboardKeyType_Struct>(UNPCBlackboardData::ExecutionTasksKey,
		FConstStructView::Make(TasksData));
	if (!bStored)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Could not write ExecutionTasks to Blackboard"), *NodeName);
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Succeeded;
}
