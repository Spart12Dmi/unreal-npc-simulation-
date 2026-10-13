// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/BaseAiController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "NPCS/BaseNPC.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseAiController, Log, All);

// Sets default values
ABaseAiController::ABaseAiController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UAIPerceptionComponent* const AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	UAISenseConfig_Sight* SightConfig = CreateSightConfig();
	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

UAISenseConfig_Sight* ABaseAiController::CreateSightConfig()
{
	/* enable npc to detect objects by sight */
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(MaxAge);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	return SightConfig;
}

// Called when the game starts or when spawned
void ABaseAiController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree && !RunBehaviorTree(BehaviorTree))
	{
		UE_LOG(LogBaseAiController, Warning, TEXT("%s failed to start behavior tree %s."),
		       *GetName(), *GetNameSafe(BehaviorTree));
	}
}

void ABaseAiController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	//set possessed npc
	NPCOwner = Cast<ABaseNPC>(InPawn);
}
// Called every frame
void ABaseAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ABaseNPC* ABaseAiController::GetNPCOwner() const
{
	return NPCOwner;
}
