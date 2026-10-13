#include "NPCS/BehaviorTree_Tasks/ImplementTaskTree/BTTask_IncrementTaskIndex.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "NPCS/Blackboard/ExecutionTasksData.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"
#include "StructUtils/StructView.h"

UBTTask_IncrementTaskIndex::UBTTask_IncrementTaskIndex()
{
	NodeName = TEXT("Increment Task Index");
}

EBTNodeResult::Type UBTTask_IncrementTaskIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard) ||
		!Blackboard->IsKeyOfType<UBlackboardKeyType_Int>(UNPCBlackboardData::CurrentTaskIndexKey) ||
		!Blackboard->IsKeyOfType<UBlackboardKeyType_Struct>(UNPCBlackboardData::ExecutionTasksKey))
	{
		return EBTNodeResult::Failed;
	}

	const FConstStructView Value =
		Blackboard->GetValue<UBlackboardKeyType_Struct>(UNPCBlackboardData::ExecutionTasksKey);
	const FExecutionTasksData* TasksData = Value.GetPtr<const FExecutionTasksData>();
	if (!TasksData || TasksData->Tasks.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	const int32 CurrentIndex = Blackboard->GetValueAsInt(UNPCBlackboardData::CurrentTaskIndexKey);
	if (!TasksData->Tasks.IsValidIndex(CurrentIndex))
	{
		return EBTNodeResult::Failed;
	}

	// Wrap to the first task after the last one; the nonempty check prevents division by zero.
	const int32 NextIndex = (CurrentIndex + 1) % TasksData->Tasks.Num();
	Blackboard->SetValueAsInt(UNPCBlackboardData::CurrentTaskIndexKey, NextIndex);
	return EBTNodeResult::Succeeded;
}
