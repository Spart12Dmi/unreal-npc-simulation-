#include "NPCS/BehaviorTreeHelpers.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Core/Subsystems/WorldTimeSubsystem.h"
#include "DataAssets/ScheduleTemplateDataAsset.h"
#include "Engine/World.h"
#include "NPCS/BaseAiController.h"

bool NPCBehaviorTree::TryGetContext(UBehaviorTreeComponent& OwnerComp, FContext& OutContext)
{
	OutContext = {};
	ABaseAiController* Controller = Cast<ABaseAiController>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		UE_LOG(LogTemp, Error, TEXT("Expected an ABaseAiController owner"));
		return false;
	}

	OutContext.NPC = Controller->GetNPCOwner();
	if (!OutContext.NPC)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller has no NPC owner"));
		return false;
	}

	OutContext.Blackboard = Controller->GetBlackboardComponent();
	if (!OutContext.Blackboard)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller has no Blackboard component"));
		return false;
	}

	return true;
}

bool NPCBehaviorTree::TryGetScheduledActivity(const UBehaviorTreeComponent& OwnerComp,
	const FContext& Context, FGameplayTag& OutActivityTag)
{
	OutActivityTag = FGameplayTag::EmptyTag;
	const UScheduleTemplateDataAsset* Schedule = Context.NPC->GetNPCSchedule();
	if (!Schedule)
	{
		UE_LOG(LogTemp, Error, TEXT("NPC has no schedule asset"));
		return false;
	}

	const UWorld* World = OwnerComp.GetWorld();
	const UWorldTimeSubsystem* TimeSubsystem = World ? World->GetSubsystem<UWorldTimeSubsystem>() : nullptr;
	if (!TimeSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("World time subsystem is unavailable"));
		return false;
	}

	OutActivityTag = Schedule->GetActivationTagAtTime(TimeSubsystem->GetWorldTime());
	return true;
}
