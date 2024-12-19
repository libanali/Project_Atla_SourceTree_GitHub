// Fill out your copyright notice in the Description page of Project Settings.


#include "Command_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Ren_Low_Poly_Character.h"



void UCommand_Menu_Widget::NativeOnInitialized()
{

    // Set initial state to Main Menu
    CurrentMenuState = ECommandMenuState::MainMenu;

    // Bind button click events to transition methods
    if (ItemsButton)
    {
        ItemsButton->OnClicked.AddDynamic(this, &UCommand_Menu_Widget::OnItemsButtonClicked);
        ItemsButton->OnHovered.AddDynamic(this, &UCommand_Menu_Widget::OnItemsHovered);
    }

    if (TechniquesButton)
    {
        TechniquesButton->OnClicked.AddDynamic(this, &UCommand_Menu_Widget::OnTechniquesButtonClicked);
        TechniquesButton->OnHovered.AddDynamic(this, &UCommand_Menu_Widget::OnTechniquesHovered);

    }

    if(ElementalButton)
    {
    
        ElementalButton->OnClicked.AddDynamic(this, &UCommand_Menu_Widget::OnElementalAttacksClicked);
        ElementalButton->OnHovered.AddDynamic(this, &UCommand_Menu_Widget::OnElementalHovered);

    }


    // Ensure the command image is visible at the start
    CommandMenuIcon->SetVisibility(ESlateVisibility::Visible); 

    // Initially hide the buttons
    ItemsButton->SetVisibility(ESlateVisibility::Hidden);
    TechniquesButton->SetVisibility(ESlateVisibility::Hidden);
    ElementalButton->SetVisibility(ESlateVisibility::Hidden);

    ItemsButton->SetKeyboardFocus();

    WidgetSwitcher->SetActiveWidgetIndex(0);


    if (InventoryWidgetClass)
    {
        InventoryWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
        if (InventoryWidgetInstance)
        {
            InventoryWidgetInstance->AddToViewport();
            InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden); // Initially hidden
        }
    }


    if (TechniquesWidgetClass)
    {
        TechniquesWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), TechniquesWidgetClass);
        if (TechniquesWidgetInstance)
        {
            TechniquesWidgetInstance->AddToViewport();
            TechniquesWidgetInstance->SetVisibility(ESlateVisibility::Hidden); // Initially hidden
        }
    }


    if (ElementalAttacksWidgetClass)
    {
        ElementalAttacksWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ElementalAttacksWidgetClass);
        if (ElementalAttacksWidgetInstance)
        {
            ElementalAttacksWidgetInstance->AddToViewport();
            ElementalAttacksWidgetInstance->SetVisibility(ESlateVisibility::Hidden); // Initially hidden
        }
    }




}


void UCommand_Menu_Widget::ShowItemsMenu()
{


    // Hide main command buttons (Items/Techniques)
    ItemsButton->SetVisibility(ESlateVisibility::Hidden);
    TechniquesButton->SetVisibility(ESlateVisibility::Hidden);
    CommandMenuIcon->SetVisibility(ESlateVisibility::Hidden);
    ElementalButton->SetVisibility(ESlateVisibility::Hidden);



}

void UCommand_Menu_Widget::ShowTechniquesMenu()
{

    ItemsButton->SetVisibility(ESlateVisibility::Hidden);
    TechniquesButton->SetVisibility(ESlateVisibility::Hidden);
    ElementalButton->SetVisibility(ESlateVisibility::Hidden);


}



void UCommand_Menu_Widget::ShowElementalAttacksMenu()
{

    ItemsButton->SetVisibility(ESlateVisibility::Hidden);
    TechniquesButton->SetVisibility(ESlateVisibility::Hidden);
    ElementalButton->SetVisibility(ESlateVisibility::Hidden);
}



void UCommand_Menu_Widget::ReturnToMainMenu()
{
    if (WidgetSwitcher)
    {
        // Set the active widget back to the main menu
        WidgetSwitcher->SetActiveWidgetIndex(0);  // Index 0 corresponds to the Main menu
        CurrentMenuState = ECommandMenuState::MainMenu;
    }

}

void UCommand_Menu_Widget::HandleBackNavigation()
{

    if (CurrentMenuState == ECommandMenuState::ItemsMenu || CurrentMenuState == ECommandMenuState::TechniquesMenu)
    {
        // If in a submenu, go back to the main command menu
        ReturnToMainMenu();
    }
    else if (CurrentMenuState == ECommandMenuState::MainMenu)
    {
        // If in the main menu, hide the command menu (return to gameplay)
        SetVisibility(ESlateVisibility::Hidden);
    }


}

void UCommand_Menu_Widget::OnItemsButtonClicked()
{


    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetOwningPlayerPawn());
    if (PlayerCharacter)
    {
        PlayerCharacter->OpenInventory();
    }



}

void UCommand_Menu_Widget::OnTechniquesButtonClicked()
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetOwningPlayerPawn());
    if (PlayerCharacter)
    {
        PlayerCharacter->OpenTechniques();
    }

}

void UCommand_Menu_Widget::OnElementalAttacksClicked()
{



    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetOwningPlayerPawn());
    if (PlayerCharacter)
    {
        PlayerCharacter->OpenElementalAttacks();
    }



}

void UCommand_Menu_Widget::OnItemsHovered()
{

    if (ItemsButton)
    {
        InformationText->SetText(FText::FromString(TEXT("Browse your collected items.")));
    }


}



void UCommand_Menu_Widget::OnTechniquesHovered()
{


    if (TechniquesButton)
    {
        InformationText->SetText(FText::FromString(TEXT("Perform a powerful technique.")));
    }

}



void UCommand_Menu_Widget::OnElementalHovered()
{


    if (ElementalButton)
    {
        InformationText->SetText(FText::FromString(TEXT("Use elemental-type attacks.")));
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



void UCommand_Menu_Widget::UpdateVisibilityBasedOnIndex(int CurrentIndex)
{

    if (InventoryWidgetInstance)
    {
        if (CurrentIndex == 2) // If in inventory index
        {
            InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }


    if (TechniquesWidgetInstance)
    {
        if (CurrentIndex == 3) // Techniques index
        {
            TechniquesWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            TechniquesWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }

}

void UCommand_Menu_Widget::Tick(float DeltaTime)
{



}

void UCommand_Menu_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);

    // Check if ItemsButton has keyboard focus
    if (ItemsButton && ItemsButton->HasKeyboardFocus())
    {
        InformationText->SetText(FText::FromString(TEXT("Browse your collected items.")));
    }
    else if (TechniquesButton && TechniquesButton->HasKeyboardFocus())
    {
        InformationText->SetText(FText::FromString(TEXT("Perform a powerful technique.")));
    }
    else if (ElementalButton && ElementalButton->HasKeyboardFocus())
    {
        InformationText->SetText(FText::FromString(TEXT("Use elemental-type attacks.")));
    }


}




