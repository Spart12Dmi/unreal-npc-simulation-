// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BehaviorTree_Tasks/BTTask_ReserveSmartInteraction.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Struct.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "Core/Subsystems/SmartInteractionSubsystem.h"
#include "Engine/World.h"
#include "NPCS/BehaviorTreeHelpers.h"
#include "NPCS/Blackboard/ExecutionTasksData.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"
#include "StructUtils/StructView.h"

UBTTask_ReserveSmartInteraction::UBTTask_ReserveSmartInteraction()
{
	NodeName = TEXT("Reserve Smart Interaction");
}

void UBTTask_ReserveSmartInteraction::ReleasePreviousInteractionComponent(UBlackboardComponent* BBComponent,
	ABaseNPC* NPCOwner)
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
	NPCBehaviorTree::FContext Context;
	if (!NPCBehaviorTree::TryGetContext(OwnerComp, Context))
	{
		return EBTNodeResult::Failed;
	}

	FGameplayTag SmartObjectTag;
	if (!NPCBehaviorTree::TryGetScheduledActivity(OwnerComp, Context, SmartObjectTag))
	{
		return EBTNodeResult::Failed;
	}

	const UWorld* World = OwnerComp.GetWorld();
	USmartInteractionSubsystem* SmartInteractionSubsystem = World ? World->GetSubsystem<USmartInteractionSubsystem>() : nullptr;
	if (!SmartInteractionSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Smart interaction subsystem is unavailable"), *NodeName);
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComponent = Context.Blackboard;
	// Release before searching so the previous component's slot is available again.
	ReleasePreviousInteractionComponent(BBComponent, Context.NPC);
	// UE 5.7's ClearValue skips instanced struct keys because IsEmpty sees their zero-sized buffer.
	const FExecutionTasksData EmptyTasks;
	BBComponent->SetValue<UBlackboardKeyType_Struct>(UNPCBlackboardData::ExecutionTasksKey,
		FConstStructView::Make(EmptyTasks));
	BBComponent->SetValueAsInt(UNPCBlackboardData::CurrentTaskIndexKey, 0);

	const TWeakObjectPtr<USmartInteractionComponent> SmartObject =
		SmartInteractionSubsystem->FindAppropriateSmartObject(SmartObjectTag);
	USmartInteractionComponent* SmartObjectComponent = SmartObject.Get();
	if (!IsValid(SmartObjectComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No available smart interaction for tag '%s'"),
			*NodeName, *SmartObjectTag.ToString());
		return EBTNodeResult::Failed;
	}

	SmartObjectComponent->ReserveSmartObject(Context.NPC);
	BBComponent->SetValueAsObject(UNPCBlackboardData::SelectedInteractionKey, SmartObjectComponent);
	return EBTNodeResult::Succeeded;
}

