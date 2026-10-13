// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SimulationWorldGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONWORD_API ASimulationWorldGameMode : public AGameMode
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> WidgetClass;
};
