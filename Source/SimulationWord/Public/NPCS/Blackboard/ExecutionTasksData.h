#pragma once

#include "CoreMinimal.h"
#include "ExecutionTasksData.generated.h"

class UBasedInteractionTask;

/** Task references stored in the ExecutionTasks Blackboard struct key. */
USTRUCT(BlueprintType)
struct SIMULATIONWORD_API FExecutionTasksData
{
	GENERATED_BODY()
	
	FExecutionTasksData() = default;
	
	// Takes ownership of the task array by moving it; the source array becomes empty.
	explicit FExecutionTasksData(TArray<TObjectPtr<UBasedInteractionTask>>&& ExecutionTasks)
		: Tasks(MoveTemp(ExecutionTasks))
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TArray<TObjectPtr<UBasedInteractionTask>> Tasks;
};
