// Fill out your copyright notice in the Description page of Project Settings.


#include "StartUp_Screen_GameMode.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"



void AStartUp_Screen_GameMode::BeginPlay()
{


	// Ensure a menu widget is shown
	if (UUserWidget* StartUpWidget = CreateWidget<UUserWidget>(GetWorld(), StartUpScreenWidgetClass))
	{
		// Add it to the viewport
		StartUpWidget->AddToViewport();

	}






}
