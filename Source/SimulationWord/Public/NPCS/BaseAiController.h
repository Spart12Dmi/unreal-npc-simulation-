// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseNPC.h"
#include "BaseAiController.generated.h"

class UAISenseConfig_Sight;
class UBehaviorTree;

UCLASS()
class SIMULATIONWORD_API ABaseAiController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseAiController();
	UAISenseConfig_Sight* CreateSightConfig();

protected:
	//Behavior tree to run
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior Tree")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Behavior Tree")
	float SightRadius{1500.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior Tree")
	float LoseSightRadius{1800.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior Tree")
	float PeripheralVisionAngleDegrees{90.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior Tree")
	float MaxAge{5.f};
	
	UPROPERTY( BlueprintReadOnly, Category = "AI|Behavior Tree")
	ABaseNPC* NPCOwner;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//getters
	ABaseNPC* GetNPCOwner() const;
};
