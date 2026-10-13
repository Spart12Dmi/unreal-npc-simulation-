#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldTimeWidget.generated.h"

class UTextBlock;
class UWorldTimeSubsystem;

/** Displays the world clock using a TextBlock provided by a Widget Blueprint. */
UCLASS()
class SIMULATIONWORD_API UWorldTimeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void UpdateWorldTimeText();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WorldTimeText;

	TWeakObjectPtr<UWorldTimeSubsystem> WorldTimeSubsystem;
	FString LastClockString;
};
