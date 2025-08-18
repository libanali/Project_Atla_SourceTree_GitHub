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


    // Only set focus if controller is connected
    if (IsControllerConnected())
    {
        if (ResumeButton)
        {
            ResumeButton->SetKeyboardFocus();
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Pause Menu: Controller detected - focus set"));
        }
    }
    else
    {
        // Clear focus if no controller
        FSlateApplication::Get().ClearKeyboardFocus();
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Pause Menu: No controller - mouse mode"));
    }


    if (YesButton)
    {
        YesButton->OnClicked.Clear();
        YesButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnYesClicked);
    }

    if (NoButton)
    {
        NoButton->OnClicked.Clear();
        NoButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnNoClicked);
    }


    if (QuitButton)
    {
        QuitButton->OnClicked.Clear();
        QuitButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnQuitClicked);
    }

    // Hide confirmation canvas initially
    if (ConfirmationCanvas)
    {
        ConfirmationCanvas->SetVisibility(ESlateVisibility::Hidden);
    }

    // Initial state
    CurrentMenuState = 0;

    SetInitialFocus();


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

        // Set input mode based on controller presence
        if (IsControllerConnected())
        {
            // UI only mode with focus on widget for controller
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(this->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputMode);

            // Set keyboard focus on the widget
            this->SetKeyboardFocus();
        }
        else
        {
            // Simple UI only mode for mouse
            PlayerController->SetInputMode(FInputModeUIOnly());
        }
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



void UPause_Menu_Widget::OnQuitClicked()
{

    UpdateMenuState(1);

}




void UPause_Menu_Widget::OnYesClicked()
{
    // Show debug message on screen
   // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ATTEMPTING LEVEL TRANSITION"));
    UE_LOG(LogTemp, Error, TEXT("OnYesClicked: Attempting transition to main menu"));

    // Cache local copy of world pointer for safety
    UWorld* CurrentWorld = GetWorld();

    if (!CurrentWorld)
    {
      //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERROR: Invalid World!"));
        UE_LOG(LogTemp, Error, TEXT("OnYesClicked: World is NULL!"));
        return;
    }

    // Close UI first
    HidePauseMenu();

    // Try to freeze game state
    UGameplayStatics::SetGamePaused(CurrentWorld, true);

    // Try to disable player input
    APlayerController* PlayerController = CurrentWorld->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->DisableInput(PlayerController);

        // Set a clean game-only input mode (sometimes helps with transitions)
        FInputModeGameOnly GameOnlyInputMode;
        PlayerController->SetInputMode(GameOnlyInputMode);
    }

    // Clean up game mode if possible
    ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(CurrentWorld->GetAuthGameMode());
    if (GameMode)
    {
      //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Preparing GameMode for transition"));
        UE_LOG(LogTemp, Warning, TEXT("Found valid GameMode, preparing for transition"));
        GameMode->PrepareForLevelTransition();
    }
    else
    {
     //   GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("WARNING: No GameMode found!"));
        UE_LOG(LogTemp, Error, TEXT("OnYesClicked: GameMode is NULL!"));
    }

    // FALLBACK OPTIONS - TRY MULTIPLE APPROACHES

    // First approach - direct transition using GetWorld
 //   GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("DIRECT TRANSITION: Using GetWorld()"));
    UE_LOG(LogTemp, Warning, TEXT("Attempting direct transition with GetWorld()"));
    UGameplayStatics::OpenLevel(CurrentWorld, FName("Main_Menu_Level"));

    // Wait a moment to see if that worked
    FPlatformProcess::Sleep(0.2f);

    // Second approach - try with GWorld if we're still here
  //  GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("FALLBACK: Using GWorld"));
    UE_LOG(LogTemp, Warning, TEXT("Attempting fallback transition with GWorld"));
    UGameplayStatics::OpenLevel(GWorld, FName("Main_Menu_Level"));
}



void UPause_Menu_Widget::OnNoClicked()
{

    UpdateMenuState(0);



}

void UPause_Menu_Widget::SetInitialFocus()
{

    // Only set focus if controller is connected
    if (IsControllerConnected())
    {
        // Use a small delay to ensure widget is fully initialized
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
                {
                    if (CurrentMenuState == 0 && ResumeButton)
                    {
                        ResumeButton->SetKeyboardFocus();
                        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                            TEXT("Pause Menu: Controller detected - Resume button focused"));
                    }
                    else if (CurrentMenuState == 1 && NoButton)
                    {
                        NoButton->SetKeyboardFocus();
                    }
                });
        }
    }
    else
    {
        // Clear focus if no controller
        FSlateApplication::Get().ClearKeyboardFocus();
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
            TEXT("Pause Menu: No controller - mouse mode"));
    }
}




void UPause_Menu_Widget::HandleGoBack()
{

    if (CurrentMenuState == 1)
    {
        // If in confirmation screen, go back to main pause menu
        UpdateMenuState(0);
        PlayBackSound();
    }
    else
    {
        // If in main pause menu, close pause menu entirely
        HidePauseMenu();
        UGameplayStatics::SetGamePaused(GetWorld(), false);
        PlayBackSound();
    }

}




void UPause_Menu_Widget::UpdateMenuState(int32 ActiveIndex)
{
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(ActiveIndex);
        CurrentMenuState = ActiveIndex;

        // Hide/show appropriate canvases
        if (ActiveIndex == 0)
        {
            // Show main pause menu
            if (ConfirmationCanvas)
                ConfirmationCanvas->SetVisibility(ESlateVisibility::Hidden);

            // Only set focus if controller is connected
            if (IsControllerConnected())
            {
                if (ResumeButton)
                {
                    ResumeButton->SetKeyboardFocus();
                }
            }
            else
            {
                // Clear focus for mouse mode
                FSlateApplication::Get().ClearKeyboardFocus();
            }

            if (PauseMenuAnimation)
                PlayAnimation(PauseMenuAnimation);
        }
        else if (ActiveIndex == 1)
        {
            // Show confirmation canvas
            if (ConfirmationCanvas)
                ConfirmationCanvas->SetVisibility(ESlateVisibility::Visible);

            // Only set focus if controller is connected
            if (IsControllerConnected())
            {
                // Set focus on No button (safer default)
                if (NoButton)
                {
                    NoButton->SetKeyboardFocus();
                }
            }
            else
            {
                // Clear focus for mouse mode
                FSlateApplication::Get().ClearKeyboardFocus();
            }
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
    if (CurrentMenuState == 0 || 1)


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


    // Optional: Monitor controller connection changes
    static bool bLastControllerState = false;
    static float TimeSinceLastCheck = 0.0f;
    TimeSinceLastCheck += InDeltaTime;

    // Check every second
    if (TimeSinceLastCheck > 1.0f)
    {
        bool bCurrentState = IsControllerConnected();
        if (bCurrentState != bLastControllerState)
        {
            // Controller state changed
            if (bCurrentState)
            {
                // Controller connected - set appropriate focus with delay
                if (GetWorld())
                {
                    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
                        {
                            if (CurrentMenuState == 0 && ResumeButton)
                            {
                                ResumeButton->SetKeyboardFocus();
                                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                                    TEXT("Controller Connected - Resume button focused"));
                            }
                            else if (CurrentMenuState == 1 && NoButton)
                            {
                                NoButton->SetKeyboardFocus();
                                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                                    TEXT("Controller Connected - No button focused"));
                            }
                        });
                }
            }
            else
            {
                // Controller disconnected - clear focus
                FSlateApplication::Get().ClearKeyboardFocus();
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
                    TEXT("Controller Disconnected - Focus Cleared"));
            }

            bLastControllerState = bCurrentState;
        }
        TimeSinceLastCheck = 0.0f;
    }
}


bool UPause_Menu_Widget::IsControllerConnected() const
{
    return FSlateApplication::Get().IsGamepadAttached();
}






