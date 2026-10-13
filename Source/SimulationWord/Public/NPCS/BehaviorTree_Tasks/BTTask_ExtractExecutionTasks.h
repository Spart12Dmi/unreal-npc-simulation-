// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ExtractExecutionTasks.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONWORD_API UBTTask_ExtractExecutionTasks : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_ExtractExecutionTasks();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
