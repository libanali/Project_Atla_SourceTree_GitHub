// Fill out your copyright notice in the Description page of Project Settings.


#include "Command_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Image.h"



void UCommand_Menu_Widget::NativeOnInitialized()
{

    // Set initial state to Main Menu
    CurrentMenuState = ECommandMenuState::MainMenu;

    // Bind button click events to transition methods
    if (ItemsButton)
    {
        ItemsButton->OnClicked.AddDynamic(this, &UCommand_Menu_Widget::ShowItemsMenu);
    }

    if (TechniquesButton)
    {
        TechniquesButton->OnClicked.AddDynamic(this, &UCommand_Menu_Widget::ShowTechniquesMenu);
    }


    // Ensure the command image is visible at the start
    CommandMenuIcon->SetVisibility(ESlateVisibility::Visible); 

    // Initially hide the buttons
    ItemsButton->SetVisibility(ESlateVisibility::Hidden);
    TechniquesButton->SetVisibility(ESlateVisibility::Hidden);

    ItemsButton->SetKeyboardFocus();

}

void UCommand_Menu_Widget::ShowItemsMenu()
{


    // Hide main command buttons (Items/Techniques)
    ItemsButton->SetVisibility(ESlateVisibility::Hidden);
    TechniquesButton->SetVisibility(ESlateVisibility::Hidden);
    CommandMenuIcon->SetVisibility(ESlateVisibility::Hidden);



}

void UCommand_Menu_Widget::ShowTechniquesMenu()
{

    ItemsButton->SetVisibility(ESlateVisibility::Hidden);
    TechniquesButton->SetVisibility(ESlateVisibility::Hidden);


}

void UCommand_Menu_Widget::ReturnToMainMenu()
{
    CurrentMenuState = ECommandMenuState::MainMenu;

    // Go back to the main menu (reveal buttons again)
    ItemsButton->SetVisibility(ESlateVisibility::Visible);
    TechniquesButton->SetVisibility(ESlateVisibility::Visible);


}

void UCommand_Menu_Widget::HandleBackNavigation()
{

    // Navigate back based on the current state
    if (CurrentMenuState == ECommandMenuState::ItemsMenu || CurrentMenuState == ECommandMenuState::TechniquesMenu)
    {
        // If in a submenu, return to the main command menu
        ReturnToMainMenu();
    }
    else if (CurrentMenuState == ECommandMenuState::MainMenu)
    {
        // If in the main menu, close the command menu and return to gameplay
        SetVisibility(ESlateVisibility::Hidden);
    }


}

void UCommand_Menu_Widget::SetCommandMenuIconVisibility(bool bIsVisible)
{

    if (CommandMenuIcon)
    {
        // Toggle visibility based on the boolean value
        if (bIsVisible)
        {
            CommandMenuIcon->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            CommandMenuIcon->SetVisibility(ESlateVisibility::Hidden);
        }
    }


}

void UCommand_Menu_Widget::Tick(float DeltaTime)
{

    if (ItemsButton->HasKeyboardFocus())

    {

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Has focud!!"));


    }



}




