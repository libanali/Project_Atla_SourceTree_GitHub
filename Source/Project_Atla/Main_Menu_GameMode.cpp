// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Menu_GameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"




void AMain_Menu_GameMode::BeginPlay()
{



	// Ensure a menu widget is shown
	if (UUserWidget* MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass))
	{
		// Add it to the viewport
		MainMenuWidget->AddToViewport();

		// Get the player controller and set the input mode to UI only
		APlayerController* PC = (UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC)
		{
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}




}
