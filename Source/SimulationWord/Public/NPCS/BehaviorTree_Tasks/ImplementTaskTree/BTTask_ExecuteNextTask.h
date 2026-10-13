// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ExecuteNextTask.generated.h"

class APawn;
class UBasedInteractionTask;
class UBehaviorTreeComponent;
enum class EInteractionTaskState : uint8;

/**
 * 
 */
UCLASS()
class SIMULATIONWORD_API UBTTask_ExecuteNextTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ExecuteNextTask();

protected:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTNodeResult::Type TaskResult) override;
	virtual void OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp) override;

private:
	void HandleTaskFinished(UBasedInteractionTask* FinishedTask, EInteractionTaskState Result);
	void ClearActiveTask(bool bCancelRunningTask);

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UBasedInteractionTask> ActiveTask;

	TWeakObjectPtr<UBehaviorTreeComponent> ExecutingTree;
	FDelegateHandle TaskFinishedHandle;
	bool bStartingTask = false;
};
