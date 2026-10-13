#pragma once

#include "CoreMinimal.h"
#include "NPCS/Tasks/BasedInteractionTask.h"
#include "PlayAnimationInteractionTask.generated.h"

class UAnimInstance;
class UAnimMontage;

UCLASS()
class SIMULATIONWORD_API UPlayAnimationInteractionTask : public UBasedInteractionTask
{
	GENERATED_BODY()

public:
	// The character's Animation Blueprint must contain a slot matching this montage.
	UPROPERTY(EditAnywhere, Category = "Task|Animation")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category = "Task|Animation", meta = (ClampMin = "0.01"))
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Task|Animation", meta = (ClampMin = "0.0", Units = "s"))
	float CancelBlendOutTime = 0.15f;

	virtual EInteractionTaskResource GetRequiredResources() const override;

protected:
	virtual void ExecuteTask() override;
	virtual void CleanupTask() override;

private:
	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* EndedMontage, bool bInterrupted);

	TWeakObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UAnimMontage> PlayingMontage;

	int32 MontageInstanceId = INDEX_NONE;
};
