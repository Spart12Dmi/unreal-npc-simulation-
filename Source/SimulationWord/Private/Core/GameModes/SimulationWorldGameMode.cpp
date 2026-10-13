// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameModes/SimulationWorldGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void ASimulationWorldGameMode::BeginPlay()
{
	Super::BeginPlay();

	//adding time widget to viewport
	if (IsValid(WidgetClass))
	{
		if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
		{
			if (UUserWidget* Widget = CreateWidget<UUserWidget>(Controller, WidgetClass))
			{
				Widget->AddToViewport();
			}
		}
	}
}
