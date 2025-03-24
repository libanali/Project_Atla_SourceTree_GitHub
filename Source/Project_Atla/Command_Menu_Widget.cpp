// Fill out your copyright notice in the Description page of Project Settings.


#include "Command_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/Application/SlateApplication.h"
#include "Ren_Low_Poly_Character.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "GenericPlatform/IInputInterface.h"



void UCommand_Menu_Widget::NativeOnInitialized()
{

    PlayAnimation(CommandMenuIcon_FadeAnim);

    // Set initial state to Main Menu
    CurrentMenuState = ECommandMenuState::MainMenu;

    CurrentText = "";
    
    if (FSlateApplication::Get().GetPlatformApplication()->IsGamepadAttached())
    {
        CurrentInputMode = EInputMode::Gamepad;
       // GEngine->AddOnScreenDebugMessage(2, 2.5f, FColor::Green, TEXT("Gamepad Connected!"));

        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            PlayerController->bShowMouseCursor = false;
        }

    }
    else
    {
        CurrentInputMode = EInputMode::Mouse;
       // GEngine->AddOnScreenDebugMessage(2, 2.5f, FColor::Black, TEXT("No Gamepad Connected!"));

        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            PlayerController->bShowMouseCursor = true;
        }
    }


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

  /*  if (ItemsButton)
    {
        InformationText->SetText(FText::FromString(TEXT("Browse your collected items.")));
    }

    */

    UpdateInformationText("Browse your collected items.");

}



void UCommand_Menu_Widget::OnTechniquesHovered()
{
    /*

    if (TechniquesButton)
    {
        InformationText->SetText(FText::FromString(TEXT("Perform a powerful technique.")));
    }
     */

    UpdateInformationText("Perform a powerful technique.");



}



void UCommand_Menu_Widget::OnElementalHovered()
{
    /*

    if (ElementalButton)
    {
        InformationText->SetText(FText::FromString(TEXT("Use elemental-type attacks.")));
    }
    */


        UpdateInformationText("Use elemental-type attacks.");

    
}




void UCommand_Menu_Widget::CheckInventoryAndSetFocus()
{

    // Get player character and check inventory
    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetOwningPlayerPawn());
    if (!PlayerCharacter || !PlayerCharacter->InventoryComponent)
        return;

    if (ItemsButton && TechniquesButton)
    {
        if (PlayerCharacter->InventoryComponent->bIsInventoryEmpty)
        {
            // Inventory is empty
            ItemsButton->SetRenderOpacity(0.5f);
            ItemsButton->SetIsEnabled(false);
            ItemsButton->IsFocusable = false;

            // Focus on Techniques button instead
            TechniquesButton->SetKeyboardFocus();
        }
        else
        {
            // Inventory has items
            ItemsButton->SetRenderOpacity(1.0f);
            ItemsButton->SetIsEnabled(true);
            ItemsButton->IsFocusable = true;

            // Set focus to Items button if appropriate
            if (WidgetSwitcher && WidgetSwitcher->GetActiveWidgetIndex() == 1)  // Main menu index
            {
                ItemsButton->SetKeyboardFocus();
            }
        }
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

void UCommand_Menu_Widget::PlayNavigationSound()
{

    if (NavigationSound)

    {

        UGameplayStatics::PlaySound2D(GetWorld(), NavigationSound);

    }

}






void UCommand_Menu_Widget::UpdateInformationText(const FString& NewText)
{


    if (CurrentText != NewText) // Avoid redundant updates
    {
        CurrentText = NewText;
        if (InformationText)
        {
            InformationText->SetText(FText::FromString(NewText));
        }
    }



}









void UCommand_Menu_Widget::Tick(float DeltaTime)
{



}



void UCommand_Menu_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);


    if (CurrentInputMode == EInputMode::Gamepad)
    {
        // Gamepad: Check for keyboard focus
        if (ItemsButton && ItemsButton->HasKeyboardFocus())
        {
            UpdateInformationText("Browse your collected items.");
        }
        else if (TechniquesButton && TechniquesButton->HasKeyboardFocus())
        {
            UpdateInformationText("Perform a powerful technique.");
        }
        else if (ElementalButton && ElementalButton->HasKeyboardFocus())
        {
            UpdateInformationText("Use elemental-type attacks.");
        }
    }
    else if (CurrentInputMode == EInputMode::Mouse)
    {
        // Mouse: Check for hover
        if (ItemsButton && ItemsButton->IsHovered())
        {
            UpdateInformationText("Browse your collected items.");
        }
        else if (TechniquesButton && TechniquesButton->IsHovered())
        {
            UpdateInformationText("Perform a powerful technique.");
        }
        else if (ElementalButton && ElementalButton->IsHovered())
        {
            UpdateInformationText("Use elemental-type attacks.");
        }
    }
    
}

FNavigationReply UCommand_Menu_Widget::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    EUINavigation Direction = InNavigationEvent.GetNavigationType();


    // Play sound for navigation keys
    if (Direction == EUINavigation::Up ||
        Direction == EUINavigation::Down)
    {
       PlayNavigationSound();
    }


    return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
}




