#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "NPCBlackboardData.generated.h"

/** Blackboard key definitions used by NPC behavior trees. */
UCLASS(BlueprintType, meta = (DisplayName = "NPC Blackboard"))
class SIMULATIONWORD_API UNPCBlackboardData : public UBlackboardData
{
	GENERATED_BODY()

public:
	explicit UNPCBlackboardData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static const FName SelectedInteractionKey;
	static const FName ExecutionTasksKey;
	static const FName CurrentTaskIndexKey;
};
