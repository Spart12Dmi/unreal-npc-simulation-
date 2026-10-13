// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NPCS/BaseNPC.h"
#include "UObject/Object.h"
#include "SmartInteractionComponent.generated.h"

class UBasedInteractionTask;
/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class SIMULATIONWORD_API USmartInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	USmartInteractionComponent();

public:
	virtual void InitializeComponent() override;
	
	FGameplayTag GetGameplayTag() const
	{
		return SmartTag;
	}
	
	bool IsComponentAccessible() const;
	
	void ReserveSmartObject( ABaseNPC* BaseNPC) ;
	
	// Releases only this NPC's slot; other workers keep their reservations.
	void ReleaseSmartObject(ABaseNPC* BaseNPC);
	
	TArray<TObjectPtr<UBasedInteractionTask>> GetInteractionTasks() const;

private:
	//make subsystem know that new smart object was created
	void RegisterObject();
	
	UPROPERTY(EditInstanceOnly, Category = "SMART", meta = (AllowPrivateAccess = true))
	FGameplayTag SmartTag;
	
	TArray<TWeakObjectPtr<AActor>> Workers;
	
	UPROPERTY(EditInstanceOnly, Category = "SMART", meta = (AllowPrivateAccess = true))
	int32 MaxWorkers{1};
	
	UPROPERTY(EditAnywhere, Instanced, Category = "Interaction")
	TArray<TObjectPtr<UBasedInteractionTask>> InteractionTasks;

};
