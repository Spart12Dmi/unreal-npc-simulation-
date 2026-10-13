#include "NPCS/Tasks/MoveToInteractionTask.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"

EInteractionTaskResource UMoveToInteractionTask::GetRequiredResources() const
{
	return EInteractionTaskResource::Movement;
}

void UMoveToInteractionTask::ExecuteTask()
{
	const APawn* TaskPawn = GetPawn();
	//initialize controller and path following movement
	AAIController* Controller = TaskPawn ? Cast<AAIController>(TaskPawn->GetController()) : nullptr;
	UPathFollowingComponent* Following = Controller ? Controller->GetPathFollowingComponent() : nullptr;
	if (!IsValid(Following) || Destination.ContainsNaN() || !FMath::IsFinite(AcceptanceRadius) ||
		AcceptanceRadius < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToInteractionTask::ExecuteTask: Invalid parameters"));
		Finish(EInteractionTaskState::Failed);
		return;
	}

	PathFollowingComponent = Following;
	MoveFinishedHandle = Following->OnRequestFinished.AddUObject(this, &UMoveToInteractionTask::HandleMoveFinished);

	FAIMoveRequest Request(Destination);
	Request.SetAcceptanceRadius(AcceptanceRadius);
	Request.SetReachTestIncludesAgentRadius(false);
	Request.SetReachTestIncludesGoalRadius(false);
	Request.SetAllowPartialPath(false);
	const FPathFollowingRequestResult Result = Controller->MoveTo(Request);
	MoveRequestId = Result.MoveId;

	if (Result.Code == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		Finish(EInteractionTaskState::Succeeded);
	}
	else if (Result.Code != EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToInteractionTask::ExecuteTask: Request failed"));
		Finish(EInteractionTaskState::Failed);
	}
}

void UMoveToInteractionTask::HandleMoveFinished(FAIRequestID RequestId, const FPathFollowingResult& Result)
{
	if (MoveRequestId.IsValid() && RequestId == MoveRequestId)
	{
		Finish(Result.IsSuccess() ? EInteractionTaskState::Succeeded : EInteractionTaskState::Failed);
	}
}

void UMoveToInteractionTask::CleanupTask()
{
	if (UPathFollowingComponent* Following = PathFollowingComponent.Get())
	{
		Following->OnRequestFinished.Remove(MoveFinishedHandle);
		if (MoveRequestId.IsValid() && Following->GetCurrentRequestId() == MoveRequestId &&
			GetState() != EInteractionTaskState::Succeeded)
		{
			// Abort only this task's request, leaving any replacement request alone.
			Following->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished, MoveRequestId);
		}
	}

	MoveFinishedHandle.Reset();
	MoveRequestId = FAIRequestID::InvalidRequest;
	PathFollowingComponent.Reset();
}
