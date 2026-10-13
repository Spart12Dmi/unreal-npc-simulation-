// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BehaviorTreeDecorators/BTDecorator_IsInteractionMatchSchedule.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "NPCS/BehaviorTreeHelpers.h"
#include "NPCS/Blackboard/NPCBlackboardData.h"

UBTDecorator_IsInteractionMatchSchedule::UBTDecorator_IsInteractionMatchSchedule(
	const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Is Interaction matches Schedule");
}

bool UBTDecorator_IsInteractionMatchSchedule::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	NPCBehaviorTree::FContext Context;
	if (!NPCBehaviorTree::TryGetContext(OwnerComp, Context))
	{
		return false;
	}

	FGameplayTag SmartObjectTag;
	if (!NPCBehaviorTree::TryGetScheduledActivity(OwnerComp, Context, SmartObjectTag))
	{
		return false;
	}

	const USmartInteractionComponent* SmartInteractionComponent =
		Cast<USmartInteractionComponent>(Context.Blackboard->GetValueAsObject(UNPCBlackboardData::SelectedInteractionKey));
	if (!IsValid(SmartInteractionComponent))
	{
		return true;
	}
	const bool MatchesTag = SmartObjectTag == SmartInteractionComponent->GetGameplayTag();
	return !MatchesTag;
}
