// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Menu_Widget.h"
#include "Components/Button.h"
#include "Kismet/Gameplaystatics.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"


void UMain_Menu_Widget::NativeConstruct()
{

    // Bind buttons to functions
    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnPlayClicked);
    }
    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnBackClicked);
    }



    if (PressAnyButtonText)

    {
        PlayAnimation(FadeAnimation, 1.0f, 0);
    }


   
   APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)


    {

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("player controller active!"));


    }


    this->SetIsFocusable(true);

//    SwitchToWeaponSelectMenu();
    
    bIsOnTitleScreen = true;
    bHasSetFocusForSwordButton = false;
    
}






void UMain_Menu_Widget::OnPlayClicked()
{

    if (WidgetSwitcher)
    {
        // Switch to Weapon Select (index 2)
        WidgetSwitcher->SetActiveWidgetIndex(2);
    }


}




void UMain_Menu_Widget::OnBackClicked()
{

    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        // Logic to go back to the previous menu
        if (CurrentIndex > 0)
        {
            WidgetSwitcher->SetActiveWidgetIndex(CurrentIndex - 1);
        }
    }



}



void UMain_Menu_Widget::HandleGoBack()
{

    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        // Only go back if the current index is greater than 1
        if (CurrentIndex > 1)
        {
            WidgetSwitcher->SetActiveWidgetIndex(CurrentIndex - 1);
        }
        else
        {
            // Optionally log or handle the case when trying to go back from the main menu
            UE_LOG(LogTemp, Warning, TEXT("Cannot go back further from the main menu!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WidgetSwitcher is null!"));
    }

}



void UMain_Menu_Widget::SwitchToMainMenu()
{

    if (WidgetSwitcher && bIsOnTitleScreen)

    {
        WidgetSwitcher->SetActiveWidgetIndex(1); // Switch to Main Menu (index 1)
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Main Menu!"));


        if (PlayButton)

        {

            PlayButton->SetKeyboardFocus();

        }

        bIsOnTitleScreen = false;
    }


}





void UMain_Menu_Widget::SwitchToWeaponSelectMenu()
{

    // Check if we're on Widget 2 (index 3)
    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        if (CurrentIndex == 2 && !bHasSetFocusForSwordButton)
        {
            // Set focus on SwordButton if not already focused
            if (SwordButton)
            {
                SwordButton->SetKeyboardFocus();
                bHasSetFocusForSwordButton = true;  // Mark that focus has been set
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Sword button focused!"));
            }
        }
        else if (CurrentIndex != 2)
        {
            // Reset the flag if we're not on widget 2
            bHasSetFocusForSwordButton = false;
        }
    }


}




FReply UMain_Menu_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{


    // Get the pressed key
    FKey PressedKey = InKeyEvent.GetKey();

    // Check for specific keys (e.g., "Gamepad Face Button Right" for PS4/5 circle or "Escape" for keyboard back)
    if (PressedKey == EKeys::Gamepad_FaceButton_Right || PressedKey == EKeys::J)
    {
        HandleGoBack(); // Handle the back button logic
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Back!"));
        return FReply::Handled();
    }


    // Check for "Any Key" or "Gamepad Face Button Bottom" for switching to the main menu
    if (PressedKey == EKeys::AnyKey || PressedKey == EKeys::Gamepad_FaceButton_Bottom)
    {
        SwitchToMainMenu(); // Handle switching to the main menu
        return FReply::Handled();
    }


    // Optionally: Handle other keys here if needed
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);



}


void UMain_Menu_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);


    SwitchToWeaponSelectMenu();

}


