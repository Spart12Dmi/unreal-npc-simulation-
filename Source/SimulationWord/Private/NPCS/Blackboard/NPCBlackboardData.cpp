#include "NPCS/Blackboard/NPCBlackboardData.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "NPCS/Blackboard/ExecutionTasksData.h"

const FName UNPCBlackboardData::SelectedInteractionKey(TEXT("SelectedInteraction"));
const FName UNPCBlackboardData::ExecutionTasksKey(TEXT("ExecutionTasks"));
const FName UNPCBlackboardData::CurrentTaskIndexKey(TEXT("CurrentTaskIndex"));

UNPCBlackboardData::UNPCBlackboardData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UBlackboardKeyType_Object* InteractionKeyType =
		CreateDefaultSubobject<UBlackboardKeyType_Object>(TEXT("SelectedInteractionKeyType"));
	InteractionKeyType->BaseClass = USmartInteractionComponent::StaticClass();

	FBlackboardEntry& InteractionEntry = Keys.AddDefaulted_GetRef();
	InteractionEntry.EntryName = SelectedInteractionKey;
	InteractionEntry.KeyType = InteractionKeyType;
	// Each NPC keeps its own selected object.
	InteractionEntry.bInstanceSynced = false;

	UBlackboardKeyType_Struct* ExecutionTasksKeyType =
		CreateDefaultSubobject<UBlackboardKeyType_Struct>(TEXT("ExecutionTasksKeyType"));
	ExecutionTasksKeyType->DefaultValue.InitializeAs<FExecutionTasksData>();

	FBlackboardEntry& ExecutionTasksEntry = Keys.AddDefaulted_GetRef();
	ExecutionTasksEntry.EntryName = ExecutionTasksKey;
	ExecutionTasksEntry.KeyType = ExecutionTasksKeyType;
	ExecutionTasksEntry.bInstanceSynced = false;

	UBlackboardKeyType_Int* TaskIndexKeyType =
		CreateDefaultSubobject<UBlackboardKeyType_Int>(TEXT("CurrentTaskIndexKeyType"));
	TaskIndexKeyType->DefaultValue = 0;
	
	//Current task index
	FBlackboardEntry& TaskIndexEntry = Keys.AddDefaulted_GetRef();
	TaskIndexEntry.EntryName = CurrentTaskIndexKey;
	TaskIndexEntry.KeyType = TaskIndexKeyType;
	TaskIndexEntry.bInstanceSynced = false;
}

