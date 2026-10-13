#include "ComponentVisualizer.h"
#include "Core/Components/SmartInteractionComponent.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "Engine/Selection.h"
#include "Modules/ModuleManager.h"
#include "NPCS/Tasks/MoveToInteractionTask.h"
#include "PrimitiveDrawingUtils.h"
#include "ScopedTransaction.h"
#include "SceneManagement.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

struct HInteractionDestinationProxy : HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HInteractionDestinationProxy(const UActorComponent* Component, int32 InTaskIndex)
		: HComponentVisProxy(Component, HPP_Foreground), TaskIndex(InTaskIndex)
	{
	}

	int32 TaskIndex;
};

IMPLEMENT_HIT_PROXY(HInteractionDestinationProxy, HComponentVisProxy);

// Move tasks are instanced UObjects, which the built-in MakeEditWidget traversal doesn't visit.
class FSmartInteractionVisualizer : public FComponentVisualizer
{
public:
	virtual bool ShouldAutoSelectElementOnHandleClick() const override
	{
		// Selecting a non-scene component hides Unreal's transform widget.
		return false;
	}

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View,
		FPrimitiveDrawInterface* PDI) override
	{
		const USmartInteractionComponent* Interaction = Cast<USmartInteractionComponent>(Component);
		if (!Interaction)
		{
			return;
		}

		const auto Tasks = Interaction->GetInteractionTasks();
		for (int32 Index = 0; Index < Tasks.Num(); ++Index)
		{
			const UMoveToInteractionTask* Task = Cast<UMoveToInteractionTask>(Tasks[Index].Get());
			if (!IsValid(Task) || Task->Destination.ContainsNaN())
			{
				continue;
			}

			const bool bSelected = EditedComponent.GetComponent() == Component && SelectedTaskIndex == Index;
			const FLinearColor Color = bSelected ? FLinearColor::Yellow : FLinearColor(0.0f, 0.8f, 1.0f);
			PDI->SetHitProxy(new HInteractionDestinationProxy(Component, Index));
			DrawWireDiamond(PDI, FTransform(Task->Destination).ToMatrixNoScale(), 20.0f, Color, SDPG_Foreground);
			PDI->DrawPoint(Task->Destination, Color, 12.0f, SDPG_Foreground);
			PDI->SetHitProxy(nullptr);

			if (const AActor* Owner = Component->GetOwner())
			{
				PDI->DrawLine(Owner->GetActorLocation(), Task->Destination, Color, SDPG_World);
			}
		}
	}

	virtual bool VisProxyHandleClick(FEditorViewportClient* ViewportClient, HComponentVisProxy* Proxy,
		const FViewportClick& Click) override
	{
		if (!Proxy || !Proxy->IsA(HInteractionDestinationProxy::StaticGetType()))
		{
			return false;
		}

		const auto* DestinationProxy = static_cast<HInteractionDestinationProxy*>(Proxy);
		const auto* Component = Cast<USmartInteractionComponent>(Proxy->Component.Get());
		if (!Component)
		{
			return false;
		}

		// Keep an actor selected so the editor draws the widget, even if the component
		// was selected in Details before clicking the destination marker.
		if (GEditor && Component->GetOwner())
		{
			GEditor->GetSelectedComponents()->DeselectAll();
			GEditor->SelectActor(Component->GetOwner(), true, false);
			GEditor->NoteSelectionChange();
		}

		EndEditing();
		EditedComponent = FComponentPropertyPath(Component);
		SelectedTaskIndex = DestinationProxy->TaskIndex;
		return GetSelectedTask() != nullptr;
	}

	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override
	{
		if (const UMoveToInteractionTask* Task = GetSelectedTask())
		{
			OutLocation = Task->Destination;
			return true;
		}
		return false;
	}

	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport,
		FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override
	{
		UMoveToInteractionTask* Task = GetSelectedTask();
		UActorComponent* Component = GetEditedComponent();
		if (!Task || !Component || ViewportClient->GetCurrentWidgetAxis() == EAxisList::None)
		{
			return false;
		}
		if (DeltaTranslate.IsNearlyZero())
		{
			return true;
		}

		if (!Transaction)
		{
			Transaction = MakeUnique<FScopedTransaction>(NSLOCTEXT("SimulationWord", "MoveInteractionDestination",
				"Move Interaction Destination"));
		}
		Component->Modify();
		Task->SetFlags(RF_Transactional);
		Task->Modify();
		Task->Destination += DeltaTranslate;

		FPropertyChangedEvent ChangeEvent(FindFProperty<FProperty>(UMoveToInteractionTask::StaticClass(),
			GET_MEMBER_NAME_CHECKED(UMoveToInteractionTask, Destination)), EPropertyChangeType::Interactive);
		Task->PostEditChangeProperty(ChangeEvent);
		return true;
	}

	virtual void TrackingStopped(FEditorViewportClient* ViewportClient, bool bDidMove) override
	{
		Transaction.Reset();
	}

	virtual void EndEditing() override
	{
		Transaction.Reset();
		EditedComponent.Reset();
		SelectedTaskIndex = INDEX_NONE;
	}

	virtual UActorComponent* GetEditedComponent() const override
	{
		return EditedComponent.GetComponent();
	}

private:
	UMoveToInteractionTask* GetSelectedTask() const
	{
		const auto* Component = Cast<USmartInteractionComponent>(GetEditedComponent());
		if (!Component)
		{
			return nullptr;
		}
		const auto Tasks = Component->GetInteractionTasks();
		return Tasks.IsValidIndex(SelectedTaskIndex)
			? Cast<UMoveToInteractionTask>(Tasks[SelectedTaskIndex].Get()) : nullptr;
	}

	FComponentPropertyPath EditedComponent;
	int32 SelectedTaskIndex = INDEX_NONE;
	TUniquePtr<FScopedTransaction> Transaction;
};

class FSimulationWordEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		if (GUnrealEd)
		{
			Visualizer = MakeShared<FSmartInteractionVisualizer>();
			GUnrealEd->RegisterComponentVisualizer(USmartInteractionComponent::StaticClass()->GetFName(), Visualizer);
			Visualizer->OnRegister();
		}
	}

	virtual void ShutdownModule() override
	{
		if (Visualizer)
		{
			Visualizer->EndEditing();
			if (GUnrealEd)
			{
				GUnrealEd->UnregisterComponentVisualizer(USmartInteractionComponent::StaticClass()->GetFName());
			}
			Visualizer.Reset();
		}
	}

private:
	TSharedPtr<FComponentVisualizer> Visualizer;
};

IMPLEMENT_MODULE(FSimulationWordEditorModule, SimulationWordEditor);
