// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Menu_PlayerController.h"

void AMain_Menu_PlayerController::BeginPlay()
{

    Super::BeginPlay();

    // Set input mode to UI
    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = true;


}

void AMain_Menu_PlayerController::SetupInputComponent()
{


    // Bind the back button action
    InputComponent->BindAction("Roll Dodge or Back", IE_Pressed, this, &AMain_Menu_PlayerController::GoBack);


}

void AMain_Menu_PlayerController::GoBack()
{






}
