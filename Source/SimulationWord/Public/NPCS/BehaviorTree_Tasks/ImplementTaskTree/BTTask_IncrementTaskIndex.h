#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_IncrementTaskIndex.generated.h"

/** Advances the Blackboard task index, wrapping to zero after the last task. */
UCLASS()
class SIMULATIONWORD_API UBTTask_IncrementTaskIndex : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_IncrementTaskIndex();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
