#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "NPCS/Tasks/BasedInteractionTask.h"
#include "MoveToInteractionTask.generated.h"

class UPathFollowingComponent;
struct FPathFollowingResult;

UCLASS()
class SIMULATIONWORD_API UMoveToInteractionTask : public UBasedInteractionTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Task|Movement")
	FVector Destination = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Task|Movement", meta = (ClampMin = "0.0", Units = "cm"))
	float AcceptanceRadius = 50.0f;

	virtual EInteractionTaskResource GetRequiredResources() const override;

protected:
	virtual void ExecuteTask() override;
	virtual void CleanupTask() override;

private:
	void HandleMoveFinished(FAIRequestID RequestId, const FPathFollowingResult& Result);

	TWeakObjectPtr<UPathFollowingComponent> PathFollowingComponent;
	FDelegateHandle MoveFinishedHandle;
	FAIRequestID MoveRequestId = FAIRequestID::InvalidRequest;
};
