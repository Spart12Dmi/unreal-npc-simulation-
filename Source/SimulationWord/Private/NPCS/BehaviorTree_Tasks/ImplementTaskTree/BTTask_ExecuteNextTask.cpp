// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BehaviorTree_Tasks/ImplementTaskTree/BTTask_ExecuteNextTask.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "NPCS/BaseAiController.h"
#include "NPCS/Blackboard/ExecutionTasksData.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"
#include "NPCS/Tasks/BasedInteractionTask.h"
#include "StructUtils/StructView.h"

UBTTask_ExecuteNextTask::UBTTask_ExecuteNextTask()
{
	NodeName = TEXT("Execute Next Task");
	// The active task and delegate belong to one NPC, not the shared tree asset.
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_ExecuteNextTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ClearActiveTask(true);

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
	const FConstStructView Value = 
		BBComponent->GetValue<UBlackboardKeyType_Struct>(UNPCBlackboardData::ExecutionTasksKey);
	const FExecutionTasksData* TasksData = Value.GetPtr<const FExecutionTasksData>();
	if (!TasksData)
	{
		return EBTNodeResult::Failed;
	}
	const TArray<TObjectPtr<UBasedInteractionTask>>& Tasks = TasksData->Tasks;
	const int32 TaskIndex = BBComponent->GetValueAsInt(UNPCBlackboardData::CurrentTaskIndexKey);
	if ( !Tasks.IsValidIndex(TaskIndex) )
	{
		UE_LOG(LogTemp, Error, TEXT("Has not enough tasks"));
		return EBTNodeResult::Failed;
	}
	ActiveTask = Tasks[TaskIndex].Get();
	if (!IsValid(ActiveTask) || ActiveTask->GetState() != EInteractionTaskState::NotStarted)
	{
		return EBTNodeResult::Failed;
	}
	ExecutingTree = &OwnerComp;
	// Bind before Start because the task may finish immediately.
	TaskFinishedHandle = ActiveTask->OnFinished.AddUObject(this, &UBTTask_ExecuteNextTask::HandleTaskFinished);
	bStartingTask = true;
	const bool bAccepted = ActiveTask->Start(NPCOwner);
	bStartingTask = false;
	//some tasks can be successfully finished in one frame
	if (bAccepted && ActiveTask->GetState() == EInteractionTaskState::Running)
	{
		return EBTNodeResult::InProgress;
	}
	const EBTNodeResult::Type Result = bAccepted && ActiveTask->GetState() == EInteractionTaskState::Succeeded
		? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	ClearActiveTask(false);
	return Result;
}

void UBTTask_ExecuteNextTask::HandleTaskFinished(UBasedInteractionTask* FinishedTask,
	EInteractionTaskState Result)
{
	// Immediate completion is returned from ExecuteTask; only finish a waiting BT node here.
	if (bStartingTask || ActiveTask.Get() != FinishedTask)
	{
		return;
	}

	UBehaviorTreeComponent* OwnerComp = ExecutingTree.Get();
	ClearActiveTask(false);
	if (IsValid(OwnerComp) && OwnerComp->GetTaskStatus(this) == EBTTaskStatus::Active)
	{
		FinishLatentTask(*OwnerComp, Result == EInteractionTaskState::Succeeded
			? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UBTTask_ExecuteNextTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ClearActiveTask(true);
	return EBTNodeResult::Aborted;
}

void UBTTask_ExecuteNextTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	ClearActiveTask(true);
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_ExecuteNextTask::OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp)
{
	ClearActiveTask(true);
	Super::OnInstanceDestroyed(OwnerComp);
}

void UBTTask_ExecuteNextTask::ClearActiveTask(bool bCancelRunningTask)
{
	UBasedInteractionTask* Task = ActiveTask.Get();
	//remove delegate
	if (IsValid(Task))
	{
		Task->OnFinished.Remove(TaskFinishedHandle);
	}
	// Reset the node's execution state.
	TaskFinishedHandle.Reset();
	ActiveTask = nullptr;
	ExecutingTree.Reset();
	if (IsValid(Task) && bCancelRunningTask &&
		Task->GetState() == EInteractionTaskState::Running)
	{
		Task->Cancel();
	}
	if (IsValid(Task))
	{
		// Reuse the same task on the next loop; its resources were cleaned up on finish or cancellation.
		Task->Reset();
	}
}
