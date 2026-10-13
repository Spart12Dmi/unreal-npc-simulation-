#include "Widgets/WorldTimeWidget.h"

#include "Components/TextBlock.h"
#include "Core/Subsystems/WorldTimeSubsystem.h"
#include "Engine/World.h"

void UWorldTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	WorldTimeSubsystem = World ? World->GetSubsystem<UWorldTimeSubsystem>() : nullptr;
	LastClockString.Reset();
	UpdateWorldTimeText();
}

void UWorldTimeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateWorldTimeText();
}

void UWorldTimeWidget::UpdateWorldTimeText()
{
	const UWorldTimeSubsystem* Subsystem = WorldTimeSubsystem.Get();
	if (!IsValid(WorldTimeText) || !IsValid(Subsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWorldTimeText: Invalid World Time"));
		return;
	}

	const FString ClockString = Subsystem->GetClockString();
	if (ClockString != LastClockString)
	{
		WorldTimeText->SetText(FText::FromString(ClockString));
		LastClockString = ClockString;
	}
}
