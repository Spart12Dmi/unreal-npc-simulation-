// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_IsInteractionMatchSchedule.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONWORD_API UBTDecorator_IsInteractionMatchSchedule : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
	UBTDecorator_IsInteractionMatchSchedule(const FObjectInitializer& ObjectInitializer);
	
	//check whether current activity matches by schedule
	virtual bool CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
