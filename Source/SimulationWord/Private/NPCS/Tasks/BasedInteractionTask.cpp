// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCS/Tasks/BasedInteractionTask.h"

#include "GameFramework/Pawn.h"

bool UBasedInteractionTask::Start(APawn* InPawn)
{
	//start task execution
	check(IsInGameThread());
	if (State != EInteractionTaskState::NotStarted || HasAnyFlags(RF_ClassDefaultObject))
	{
		return false;
	}
	State = EInteractionTaskState::Running;
	Pawn = InPawn;
	if (!IsValid(InPawn) || !InPawn->GetWorld())
	{
		Finish(EInteractionTaskState::Failed);
		return true;
	}

	ExecuteTask();
	return true;
}

void UBasedInteractionTask::Cancel()
{
	check(IsInGameThread());
	Finish(EInteractionTaskState::Cancelled);
}

void UBasedInteractionTask::Reset()
{
	check(IsInGameThread());
	if (State == EInteractionTaskState::Running)
	{
		return;
	}

	// Finish already released the task's resources through CleanupTask.
	Pawn.Reset();
	State = EInteractionTaskState::NotStarted;
}

APawn* UBasedInteractionTask::GetPawn() const
{
	return Pawn.Get();
}

EInteractionTaskResource UBasedInteractionTask::GetRequiredResources() const
{
	return EInteractionTaskResource::None;
}

UWorld* UBasedInteractionTask::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	if (const APawn* TaskPawn = Pawn.Get())
	{
		return TaskPawn->GetWorld();
	}

	const UObject* TaskOuter = GetOuter();
	return TaskOuter ? TaskOuter->GetWorld() : nullptr;
}

void UBasedInteractionTask::Finish(EInteractionTaskState Result)
{
	if (State != EInteractionTaskState::Running)
	{
		return;
	}

	check(Result == EInteractionTaskState::Succeeded || Result == EInteractionTaskState::Failed ||
		Result == EInteractionTaskState::Cancelled);
	State = Result;
	CleanupTask();
	OnFinished.Broadcast(this, State);
}

void UBasedInteractionTask::ExecuteTask()
{
}

void UBasedInteractionTask::CleanupTask()
{
}

void UBasedInteractionTask::BeginDestroy()
{
	if (State == EInteractionTaskState::Running)
	{
		// Release engine callbacks without notifying gameplay listeners during garbage collection.
		State = EInteractionTaskState::Cancelled;
		CleanupTask();
	}

	Super::BeginDestroy();
}
