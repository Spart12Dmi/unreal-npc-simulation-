#pragma once

#include "CoreMinimal.h"

class ABaseNPC;
class UBehaviorTreeComponent;
class UBlackboardComponent;
struct FGameplayTag;

namespace NPCBehaviorTree
{
	struct FContext
	{
		ABaseNPC* NPC = nullptr;
		UBlackboardComponent* Blackboard = nullptr;
	};

	bool TryGetContext(UBehaviorTreeComponent& OwnerComp, FContext& OutContext);
	// Requires a context successfully resolved by TryGetContext.
	bool TryGetScheduledActivity(const UBehaviorTreeComponent& OwnerComp, const FContext& Context,
		FGameplayTag& OutActivityTag);
}
