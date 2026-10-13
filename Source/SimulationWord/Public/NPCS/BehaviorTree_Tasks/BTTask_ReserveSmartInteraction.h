// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "NPCS/BaseNPC.h"
#include "BTTask_ReserveSmartInteraction.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONWORD_API UBTTask_ReserveSmartInteraction : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	UBTTask_ReserveSmartInteraction();

	void ReleasePreviousInteractionComponent(UBlackboardComponent*& BBComponent, ABaseNPC*& NPCOwner);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
