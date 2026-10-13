// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SmartInteractionSubsystem.generated.h"

class USmartInteractionComponent;
/**
 * 
 */
UCLASS()
class SIMULATIONWORD_API USmartInteractionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/// initialization of component on game start
	/// @param SmartObjectComponent is the component of smart object in the scene 
	void InitializeNewSmartObject(USmartInteractionComponent* SmartObjectComponent);
	
	TWeakObjectPtr<USmartInteractionComponent> FindAppropriateSmartObject(const FGameplayTag Tag);
	
private:
	TMap<FGameplayTag, TArray<TWeakObjectPtr<USmartInteractionComponent>>> SmartObjectsMap;
	
};
