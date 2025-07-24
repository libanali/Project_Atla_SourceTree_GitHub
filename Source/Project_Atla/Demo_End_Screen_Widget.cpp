// Fill out your copyright notice in the Description page of Project Settings.


#include "Demo_End_Screen_Widget.h"
#include "Components/Button.h"
#include "Components/BackgroundBlur.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "GameFramework/PlayerController.h"
#include "Animation/WidgetAnimation.h"

void UDemo_End_Screen_Widget::NativeConstruct()
{

    UE_LOG(LogTemp, Warning, TEXT("========== Demo Widget Construction =========="));
    UE_LOG(LogTemp, Warning, TEXT("BlurAnimation valid: %d"), IsValid(BlurAnimation));
    UE_LOG(LogTemp, Warning, TEXT("DemoCompleteAnimation valid: %d"), IsValid(DemoCompleteAnimation));
    UE_LOG(LogTemp, Warning, TEXT("ButtonsRevealAnimation valid: %d"), IsValid(ButtonsRevealAnimation));

    // Setup button bindings
    if (Main_Menu_Button)
    {
        Main_Menu_Button->OnClicked.AddDynamic(this, &UDemo_End_Screen_Widget::OnMainMenuClicked);
        Main_Menu_Button->SetVisibility(ESlateVisibility::Hidden);
    }

    if (Steam_Store_Button)
    {
        Steam_Store_Button->OnClicked.AddDynamic(this, &UDemo_End_Screen_Widget::OnSteamStoreClicked);
        Steam_Store_Button->SetVisibility(ESlateVisibility::Hidden);
    }

    // Initialize blur
    if (BackgroundBlur)
    {
        BackgroundBlur->SetBlurStrength(0.0f);
    }

    // Hide demo complete text initially
    if (DemoCompleteText)
    {
        DemoCompleteText->SetVisibility(ESlateVisibility::Hidden);
    }

    if (DemoScreenImage)

    {

        DemoScreenImage->SetVisibility(ESlateVisibility::Hidden);
    }

    // Make widget focusable
    SetIsFocusable(true);

    // Bind to animation finished event
    BindToAnimationFinished(BlurAnimation, OnAnimationFinishedEvent);
}



void UDemo_End_Screen_Widget::SetupDemoEnd()
{

    UE_LOG(LogTemp, Warning, TEXT("========== Setting Up Demo End =========="));

    // Start the demo end sequence with blur animation
    if (BlurAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting Demo Blur Animation"));
        OnAnimationFinishedEvent.BindDynamic(this, &UDemo_End_Screen_Widget::OnBlurComplete);
        BindToAnimationFinished(BlurAnimation, OnAnimationFinishedEvent);
        PlayAnimation(BlurAnimation);
    }

}



void UDemo_End_Screen_Widget::OnMainMenuClicked()
{

    UE_LOG(LogTemp, Warning, TEXT("Main Menu button clicked in demo"));

    // Set input mode back to game
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }

    // Force garbage collection before level change
    GEngine->ForceGarbageCollection(true);

    // Load main menu level
    UGameplayStatics::OpenLevel(GetWorld(), FName("Main_Menu_Level"), true, "?forcecleantravel=1");
}


void UDemo_End_Screen_Widget::OnSteamStoreClicked()
{

    UE_LOG(LogTemp, Warning, TEXT("Steam Store button clicked"));

}


void UDemo_End_Screen_Widget::OnBlurComplete()
{

    UE_LOG(LogTemp, Warning, TEXT("========== Demo Blur Animation Complete =========="));

    // Show demo complete text
    if (DemoCompleteText)
    {
        DemoCompleteText->SetVisibility(ESlateVisibility::Visible);
    }

    if (DemoCompleteAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting Demo Complete Text Animation"));
        OnAnimationFinishedEvent.BindDynamic(this, &UDemo_End_Screen_Widget::OnDemoTextComplete);
        BindToAnimationFinished(DemoCompleteAnimation, OnAnimationFinishedEvent);
        PlayAnimation(DemoCompleteAnimation);
    }
    else
    {
        // If no animation, go directly to next step
        OnDemoTextComplete();
    }


}



void UDemo_End_Screen_Widget::OnDemoTextComplete()
{

    UE_LOG(LogTemp, Warning, TEXT("========== Demo Text Animation Complete =========="));
    UE_LOG(LogTemp, Warning, TEXT("Starting Camera Fade Sequence"));

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        // Fade to black
        PlayerController->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0f, 1.0f), 1.0f, true, true);
        UE_LOG(LogTemp, Warning, TEXT("Started fade to black"));

        // Wait for fade to complete
        GetWorld()->GetTimerManager().SetTimer(
            TransitionTimerHandle,
            [this, PlayerController]()
            {
                UE_LOG(LogTemp, Warning, TEXT("Fade complete, fading back in"));

                // Fade from black without changing camera
                PlayerController->ClientSetCameraFade(false, FColor::Black, FVector2D(1.0f, 0.0f), 1.0f, true, true);

                // Wait for fade in, then show buttons
                GetWorld()->GetTimerManager().SetTimer(
                    ButtonsTimerHandle,
                    [this]()
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Fade in complete, showing buttons"));
                        ShowButtons();
                        BackgroundBlur->BlurStrength = 0.0f;
                    },
                    1.5f,
                    false
                );
            },
            2.0f,
            false
        );
    }

}

void UDemo_End_Screen_Widget::OnButtonsRevealComplete()
{

    UE_LOG(LogTemp, Warning, TEXT("========== Demo Buttons Reveal Complete =========="));

    // Enable UI input mode
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
       // PC->bShowMouseCursor = true;

        // Set focus to main menu button
        if (Main_Menu_Button)
        {
            Main_Menu_Button->SetKeyboardFocus();
        }

        if (DemoScreenImage)
        {
            DemoScreenImage->SetVisibility(ESlateVisibility::Visible);
        }
    }


}




void UDemo_End_Screen_Widget::ShowButtons()
{

    UE_LOG(LogTemp, Warning, TEXT("========== Showing Demo Buttons =========="));

    // Show buttons
    if (Main_Menu_Button)
    {
        Main_Menu_Button->SetVisibility(ESlateVisibility::Visible);
    }

    if (Steam_Store_Button)
    {
        Steam_Store_Button->SetVisibility(ESlateVisibility::Visible);
    }

    // Play buttons reveal animation if available
    if (ButtonsRevealAnimation)
    {
        OnAnimationFinishedEvent.BindDynamic(this, &UDemo_End_Screen_Widget::OnButtonsRevealComplete);
        BindToAnimationFinished(ButtonsRevealAnimation, OnAnimationFinishedEvent);
        PlayAnimation(ButtonsRevealAnimation);

        if (DemoScreenImageAnimation)

        {
        OnAnimationFinishedEvent.BindDynamic(this, &UDemo_End_Screen_Widget::OnButtonsRevealComplete);
        BindToAnimationFinished(DemoScreenImageAnimation, OnAnimationFinishedEvent);
        PlayAnimation(DemoScreenImageAnimation);
    }

    }
    else
    {
        // No animation, go directly to enabling input
        OnButtonsRevealComplete();
    }


}
