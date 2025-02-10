// Fill out your copyright notice in the Description page of Project Settings.


#include "Pause_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Components/WidgetSwitcher.h"

void UPause_Menu_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button click events
    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnQuitClicked);
    }

    if (ConfirmYesButton)
    {
        ConfirmYesButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitYes);
    }

    if (ConfirmNoButton)
    {
        ConfirmNoButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitNo);
    }

    // Initially hide the confirm quit canvas
    if (ConfirmQuitCanvas)
    {
        ConfirmQuitCanvas->SetVisibility(ESlateVisibility::Hidden);
    }

    // Set initial widget switcher index to main menu
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);  // Assuming MainPauseCanvas is the first widget
    }


    // Set widget as focusable
    this->SetIsFocusable(true);

}



void UPause_Menu_Widget::NativeDestruct()
{

    Super::NativeDestruct();

    // Unbind all delegates
    if (ResumeButton)
    {
        ResumeButton->OnClicked.RemoveAll(this);
    }
    if (QuitButton)
    {
        QuitButton->OnClicked.RemoveAll(this);
    }
    if (ConfirmYesButton)
    {
        ConfirmYesButton->OnClicked.RemoveAll(this);
    }
    if (ConfirmNoButton)
    {
        ConfirmNoButton->OnClicked.RemoveAll(this);
    }


}


void UPause_Menu_Widget::ShowPauseMenu()
{
    // Rebind all delegates
    if (ResumeButton)
    {
        ResumeButton->OnClicked.RemoveAll(this);
        ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);
    }
    if (QuitButton)
    {
        QuitButton->OnClicked.RemoveAll(this);
        QuitButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnQuitClicked);
    }
    if (ConfirmYesButton)
    {
        ConfirmYesButton->OnClicked.RemoveAll(this);
        ConfirmYesButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitYes);
    }
    if (ConfirmNoButton)
    {
        ConfirmNoButton->OnClicked.RemoveAll(this);
        ConfirmNoButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitNo);
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
    if (QuitButton)
    {
        QuitButton->OnClicked.RemoveAll(this);
    }
    if (ConfirmYesButton)
    {
        ConfirmYesButton->OnClicked.RemoveAll(this);
    }
    if (ConfirmNoButton)
    {
        ConfirmNoButton->OnClicked.RemoveAll(this);
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




void UPause_Menu_Widget::OnQuitClicked()
{
    if (MenuSwitcher)
    {
        // Store the last focused button
        LastFocusedButton = QuitButton;

        // Update menu state to show quit confirmation
        UpdateMenuState(1);
    }
}



void UPause_Menu_Widget::OnConfirmQuitYes()
{

    UGameplayStatics::OpenLevel(GetWorld(), FName("Main_Menu_Level"));


}




void UPause_Menu_Widget::OnConfirmQuitNo()
{

    if (MenuSwitcher)
    {
        UpdateMenuState(0);
    }

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

        case 1: // Quit Confirmation
            if (QuitConfirmAnimation)
            {
                PlayAnimation(QuitConfirmAnimation);
            }
            // Set focus on No button
            if (ConfirmNoButton)
            {
                ConfirmNoButton->SetKeyboardFocus();
            }
            break;
        }
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








