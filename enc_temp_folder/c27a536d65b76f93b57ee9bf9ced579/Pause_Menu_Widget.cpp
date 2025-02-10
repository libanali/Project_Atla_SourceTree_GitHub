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
    if (!ResumeButton || !QuitButton || !ConfirmYesButton || !ConfirmNoButton)
    {
        UE_LOG(LogTemp, Warning, TEXT("One or more buttons are nullptr in Pause Menu!"));
        return;
    }

    // Clear existing delegates
    ResumeButton->OnClicked.Clear();
    QuitButton->OnClicked.Clear();
    ConfirmYesButton->OnClicked.Clear();
    ConfirmNoButton->OnClicked.Clear();

    // Bind new delegates
    ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);
    QuitButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnQuitClicked);
    ConfirmYesButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitYes);
    ConfirmNoButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitNo);

    // Set initial widget switcher index
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);
    }

    this->SetIsFocusable(true);

}




void UPause_Menu_Widget::BindDelegates()
{

    UnbindDelegates();

    if (IsValid(ResumeButton))
        ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);

    if (IsValid(QuitButton))
        QuitButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnQuitClicked);

    if (IsValid(ConfirmYesButton))
        ConfirmYesButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitYes);

    if (IsValid(ConfirmNoButton))
        ConfirmNoButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitNo);

}



void UPause_Menu_Widget::UnbindDelegates()
{

    if (IsValid(ResumeButton))
        ResumeButton->OnClicked.RemoveAll(this);

    if (IsValid(QuitButton))
        QuitButton->OnClicked.RemoveAll(this);

    if (IsValid(ConfirmYesButton))
        ConfirmYesButton->OnClicked.RemoveAll(this);

    if (IsValid(ConfirmNoButton))
        ConfirmNoButton->OnClicked.RemoveAll(this);

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

    if (ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GameMode->StopSpawningAndDestroyEnemies();
    }
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
              //  PlayAnimation(QuitConfirmAnimation);
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








