// Fill out your copyright notice in the Description page of Project Settings.


#include "Pause_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Components/WidgetSwitcher.h"
#include "LowPoly_Survival_GameMode.h"

void UPause_Menu_Widget::NativeConstruct()
{
    Super::NativeConstruct();

   // BindDelegates();

  // Check for valid buttons
    if (!ResumeButton)
    {
        UE_LOG(LogTemp, Warning, TEXT("One or more buttons are nullptr in Pause Menu!"));
        return;
    }

    // Clear existing delegates
    ResumeButton->OnClicked.Clear();
   

    // Bind new delegates
    ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);
  
    // Set initial widget switcher index
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);
    }

    this->SetIsFocusable(true);

    ResumeButton->SetKeyboardFocus();

}




void UPause_Menu_Widget::BindDelegates()
{

    UnbindDelegates();

    if (IsValid(ResumeButton))
        ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);

  
}



void UPause_Menu_Widget::UnbindDelegates()
{

    if (IsValid(ResumeButton))
        ResumeButton->OnClicked.RemoveAll(this);

}




void UPause_Menu_Widget::ShowPauseMenu()
{
    // Rebind all delegates
    if (ResumeButton)
    {
        ResumeButton->OnClicked.RemoveAll(this);
        ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);
    }
  

    AddToViewport();

    // Show mouse cursor and set input mode
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetShowMouseCursor(true);
        PlayerController->SetInputMode(FInputModeUIOnly());
    }

    // Update menu state to show main pause menu
    UpdateMenuState(0);
}



void UPause_Menu_Widget::HidePauseMenu()
{

     // Make sure to unpause the game
        UGameplayStatics::SetGamePaused(GetWorld(), false);

    // Unbind delegates before removing from parent
    if (ResumeButton)
    {
        ResumeButton->OnClicked.RemoveAll(this);
    }
   

    RemoveFromParent();

    // Hide mouse cursor and reset input mode
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetShowMouseCursor(false);
        PlayerController->SetInputMode(FInputModeGameOnly());
    }

}



void UPause_Menu_Widget::OnResumeClicked()
{
    HidePauseMenu();
    UGameplayStatics::SetGamePaused(GetWorld(), false);

}






void UPause_Menu_Widget::PlayBackSound()
{

    if (BackSound)
    {
        UGameplayStatics::PlaySound2D(this, BackSound);
    }

}




void UPause_Menu_Widget::HandleGoBack()
{


    if (MenuSwitcher)
    {
        int32 CurrentIndex = MenuSwitcher->GetActiveWidgetIndex();
        if (CurrentIndex == 1)
        {
            UpdateMenuState(0);
        }
        else
        {
            HidePauseMenu();
            UGameplayStatics::SetGamePaused(GetWorld(), false);
        }
    }


}




void UPause_Menu_Widget::UpdateMenuState(int32 ActiveIndex)
{


    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(ActiveIndex);

        // Play the appropriate animation based on the active index
        switch (ActiveIndex)
        {
        case 0: // Main Pause Menu
            if (PauseMenuAnimation)
            {
                PlayAnimation(PauseMenuAnimation);
            }
            // Set focus on resume button
            if (ResumeButton)
            {
                ResumeButton->SetKeyboardFocus();
            }
            break;
        }
    
     }
 }




void UPause_Menu_Widget::PlayNavigationSound()
{

    if (NavigationSound)

    {

        UGameplayStatics::PlaySound2D(GetWorld(), NavigationSound);

    }


}







FReply UPause_Menu_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey PressedKey = InKeyEvent.GetKey();
    if (PressedKey == EKeys::Gamepad_FaceButton_Right ||
        PressedKey == EKeys::J ||
        PressedKey == EKeys::RightMouseButton ||
        PressedKey == EKeys::Escape)  // Added Escape key support
    {
        HandleGoBack();
        PlayBackSound();
        return FReply::Handled();
    }

    return FReply::Unhandled();
}



FNavigationReply UPause_Menu_Widget::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
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





void UPause_Menu_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);

    // Continuously check focus state
   // if (ResumeButton && ResumeButton->HasKeyboardFocus())


}








