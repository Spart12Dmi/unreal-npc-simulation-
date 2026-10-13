// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BasedInteractionTask.generated.h"

class APawn;
class UBasedInteractionTask;

UENUM(BlueprintType)
enum class EInteractionTaskState : uint8
{
	NotStarted,
	Running,
	Succeeded,
	Failed,
	Cancelled
};

// The runner will use these flags to prevent conflicting tasks from running together.
enum class EInteractionTaskResource : uint8
{
	None = 0,
	Movement = 1,
	Animation = 2
};

ENUM_CLASS_FLAGS(EInteractionTaskResource);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInteractionTaskFinished, UBasedInteractionTask*, EInteractionTaskState);

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class SIMULATIONWORD_API UBasedInteractionTask : public UObject
{
	GENERATED_BODY()

public:
	// Call Reset before reusing a completed task. Bind OnFinished before Start: completion can be immediate.
	// Returns true if accepted; the outcome is reported through OnFinished, including immediate failure.
	bool Start(APawn* InPawn);
	// Cancels a running task. No effect before Start or after completion.
	void Cancel();
	// Clears the pawn and restores NotStarted after completion or cancellation. Running tasks are unchanged.
	void Reset();

	APawn* GetPawn() const;
	EInteractionTaskState GetState() const { return State; }
	virtual EInteractionTaskResource GetRequiredResources() const;
	virtual UWorld* GetWorld() const override;

	FOnInteractionTaskFinished OnFinished;

protected:
	virtual void ExecuteTask();
	virtual void CleanupTask();
	virtual void BeginDestroy() override;

	// Only a running task can finish. Cleanup happens before notifying listeners.
	void Finish(EInteractionTaskState Result);

private:
	UPROPERTY(Transient, DuplicateTransient)
	TWeakObjectPtr<APawn> Pawn;

	UPROPERTY(Transient, DuplicateTransient)
	EInteractionTaskState State = EInteractionTaskState::NotStarted;

	bool bExecutingTask = false;
};
