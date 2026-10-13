#include "NPCS/Tasks/PlayAnimationInteractionTask.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

EInteractionTaskResource UPlayAnimationInteractionTask::GetRequiredResources() const
{
	EInteractionTaskResource Resources = EInteractionTaskResource::Animation;
	if (Montage && Montage->HasRootMotion())
	{
		Resources |= EInteractionTaskResource::Movement;
	}
	return Resources;
}

void UPlayAnimationInteractionTask::ExecuteTask()
{
	//at first detect correct mesh and owner of animation
	const ACharacter* Character = Cast<ACharacter>(GetPawn());
	const USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;
	UAnimInstance* Instance = Mesh ? Mesh->GetAnimInstance() : nullptr;
	if (!IsValid(Instance) || !IsValid(Montage) || !FMath::IsFinite(PlayRate) || PlayRate <= 0.0f ||
		!FMath::IsFinite(CancelBlendOutTime) || CancelBlendOutTime < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid play animation commands"));
		Finish(EInteractionTaskState::Failed);
		return;
	}

	AnimInstance = Instance;
	PlayingMontage = Montage;
	const float Duration = Instance->Montage_Play(PlayingMontage, PlayRate, EMontagePlayReturnType::MontageLength,
	                                              0.0f, false);
	if (Duration <= 0.0f)
	{
		Finish(EInteractionTaskState::Failed);
		return;
	}

	const FAnimMontageInstance* MontageInstance = Instance->GetActiveInstanceForMontage(PlayingMontage);
	if (!MontageInstance)
	{
		Finish(EInteractionTaskState::Failed);
		return;
	}

	MontageInstanceId = MontageInstance->GetInstanceID();
	Instance->OnMontageEnded.AddDynamic(this, &UPlayAnimationInteractionTask::HandleMontageEnded);
}

void UPlayAnimationInteractionTask::HandleMontageEnded(UAnimMontage* EndedMontage, bool bInterrupted)
{
	if (EndedMontage == PlayingMontage)
	{
		Finish(bInterrupted ? EInteractionTaskState::Failed : EInteractionTaskState::Succeeded);
	}
}

void UPlayAnimationInteractionTask::CleanupTask()
{
	if (UAnimInstance* Instance = AnimInstance.Get())
	{
		Instance->OnMontageEnded.RemoveDynamic(this, &UPlayAnimationInteractionTask::HandleMontageEnded);
		const FAnimMontageInstance* ActiveInstance = Instance->GetActiveInstanceForMontage(PlayingMontage);
		if (ActiveInstance && ActiveInstance->GetInstanceID() == MontageInstanceId &&
			GetState() != EInteractionTaskState::Succeeded)
		{
			Instance->Montage_Stop(CancelBlendOutTime, PlayingMontage);
		}
	}

	AnimInstance.Reset();
	PlayingMontage = nullptr;
	MontageInstanceId = INDEX_NONE;
}
