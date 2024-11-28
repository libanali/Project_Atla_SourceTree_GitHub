// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Over_Widget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/BackgroundBlur.h"
#include "Ren_Low_Poly_Character.h"
#include "Player_Save_Game.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Results_camera.h"
#include "LowPoly_Survival_GameMode.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"





void UGame_Over_Widget::NativeConstruct()
{
    Super::NativeConstruct();


	CurrentDisplayedScore = 0;

    // Set the blur strength to 0 initially
    if (BackgroundBlur)
    {
        BackgroundBlur->SetBlurStrength(CurrentBlurStrength);
    }

    // Start the blur intensity animation
    StartBlurEffect();



  

}




void UGame_Over_Widget::StartBlurEffect()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            BlurAnimationTimer,
            this,
            &UGame_Over_Widget::UpdateBlurEffect,
            0.08f, // Timer interval
            true,
            3.5f// Loop the timer
        );

        UE_LOG(LogTemp, Log, TEXT("Started blur effect timer."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("World context is null in StartBlurEffect."));
    }
}


void UGame_Over_Widget::UpdateBlurEffect()
{
    if (BackgroundBlur)
    {
        CurrentBlurStrength += (MaxBlurStrength / BlurDuration) * 0.08f;

        if (CurrentBlurStrength >= MaxBlurStrength)
        {
            CurrentBlurStrength = MaxBlurStrength;
            GetWorld()->GetTimerManager().ClearTimer(BlurAnimationTimer);

            UE_LOG(LogTemp, Log, TEXT("Blur effect completed."));
            PlayGameOverAnimation(); // Proceed to the next step
        }

        BackgroundBlur->SetBlurStrength(CurrentBlurStrength);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BackgroundBlur is null in UpdateBlurEffect."));
    }

}




void UGame_Over_Widget::OnGameOverTextAnimationComplete()
{

    UE_LOG(LogTemp, Log, TEXT("GameOverTextAnimation completed."));

    StartCameraFade();

}



void UGame_Over_Widget::PlayGameOverAnimation()
{
    if (GameOverTextAnimation)
    {
        float AnimationDuration = GameOverTextAnimation->GetEndTime();
        UE_LOG(LogTemp, Log, TEXT("Playing GameOverTextAnimation. Duration: %f"), AnimationDuration);

        PlayAnimation(GameOverTextAnimation);
        Game_Over_Text->SetVisibility(ESlateVisibility::Visible);

        // Set timer for the next step
        GetWorld()->GetTimerManager().SetTimer(
            AnimationTimerHandle,
            this,
            &UGame_Over_Widget::OnGameOverTextAnimationComplete,
            AnimationDuration,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameOverTextAnimation is null."));
    }

}



void UGame_Over_Widget::StartCameraFade()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        UE_LOG(LogTemp, Log, TEXT("Starting camera fade to black."));
        PlayerController->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0f, 1.0f), 1.0f, true, true);

        // Transition to the results camera after fade
        GetWorld()->GetTimerManager().SetTimer(
            AnimationTimerHandle,
            this,
            &UGame_Over_Widget::SwitchToResultsCamera,
            1.0f, // Match fade duration
            false,
            3.0f
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null in StartCameraFade."));
    }
}



void UGame_Over_Widget::SwitchToResultsCamera()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController && Results_Camera)
    {
        UE_LOG(LogTemp, Log, TEXT("Switching to Results Camera."));

        PlayerController->SetViewTargetWithBlend(
            Results_Camera,  // Camera to switch to
            0.0f,            // Blend duration
            EViewTargetBlendFunction::VTBlend_Cubic
        );

        // Fade back from black
        PlayerController->ClientSetCameraFade(false, FColor::Black, FVector2D(1.0f, 0.0f), 1.0f, true, true);

        ALowPoly_Survival_GameMode* SurvivalGameMode = Cast<ALowPoly_Survival_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));

        if (SurvivalGameMode)

        {
            SurvivalGameMode->StopSpawningAndDestroyEnemies();
        }


        ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

        if (Ren)

        {

            Ren->SpawnPlayerCharacterForRender();

        }


        // Start score fade-in animation
        GetWorld()->GetTimerManager().SetTimer(
            AnimationTimerHandle,
            this,
            &UGame_Over_Widget::PlayScoresFadeInAnimation,
            1.0f,
            false
        );


        // Start score fade-in animation
        GetWorld()->GetTimerManager().SetTimer(
            RenderImageFadeInAnimationTimerHandle,
            this,
            &UGame_Over_Widget::PlayRenderImageFadeInAnimation,
            1.0f,
            false
        );



        // Start score fade-in animation
        GetWorld()->GetTimerManager().SetTimer(
            ResultsTitleFadeInAnimationTimerHandle,
            this,
            &UGame_Over_Widget::PlayResultsTitleFadeInAnimation,
            1.0f,
            false
        );



    }
    else
    {
        if (!PlayerController)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerController is null in SwitchToResultsCamera."));
        }

        if (!Results_Camera)
        {
            UE_LOG(LogTemp, Error, TEXT("Results_Camera is null in SwitchToResultsCamera."));
        }
    }

}



void UGame_Over_Widget::PlayScoresFadeInAnimation()
{

    UE_LOG(LogTemp, Log, TEXT("Playing scores fade-in animation."));

    if (ScoreFadeInAnimation)
    {
        PlayAnimation(ScoreFadeInAnimation);

        if (FinalScoreText && HighScoreText)
        {
            FinalScoreText->SetVisibility(ESlateVisibility::Visible);
            HighScoreText->SetVisibility(ESlateVisibility::Visible);
            StartScoreAnimation();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("FinalScoreText or HighScoreText is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ScoreFadeInAnimation is null."));
    }

}



void UGame_Over_Widget::PlayResultsTitleFadeInAnimation()
{


    UE_LOG(LogTemp, Log, TEXT("Playing results title fade-in animation."));

    if (Results_Title_Animation)
    {
        PlayAnimation(Results_Title_Animation);

        if (Results_Title_Text)
        {
            Results_Title_Text->SetVisibility(ESlateVisibility::Visible);
            
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("results title text is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("results animation is null."));
    }



}



void UGame_Over_Widget::PlayRenderImageFadeInAnimation()
{


    UE_LOG(LogTemp, Log, TEXT("Playing PlayRenderImage fade-in animation."));

    if (Render_Image_Animation)
    {
        PlayAnimation(Render_Image_Animation);

        if (Render_Image)
        {
            Render_Image->SetVisibility(ESlateVisibility::Visible);

        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Render Image is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayRenderImage fade-in animation is null."));
    }


}



void UGame_Over_Widget::PlayButtonsFadeInAnimation()
{


    UE_LOG(LogTemp, Log, TEXT("Playing PlayRenderImage fade-in animation."));

    if (Buttons_Animation)
    {
        PlayAnimation(Buttons_Animation);

        if (MainMenu_Button && Retry_Button)
        {
            MainMenu_Button->SetVisibility(ESlateVisibility::Visible);
            Retry_Button->SetVisibility(ESlateVisibility::Visible);


        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Buttons is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Buttons fade-in animation is null."));
    }



}



void UGame_Over_Widget::SetInputModeToUI()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerController)
    {
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;
    }


}



void UGame_Over_Widget::StartEXPBarFill(float AddedEXP)
{
    if (AddedEXP <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid AddedEXP value: %.2f"), AddedEXP);
        return;
    }

    RemainingEXPToAdd = AddedEXP;

    // Ensure progress bar starts at the correct value and becomes visible
    if (EXPProgressBar)
    {
        float InitialPercent = WeaponProficiency.CurrentEXP / WeaponProficiency.EXPToNextLevel;
        EXPProgressBar->SetPercent(InitialPercent);
        EXPProgressBar->SetVisibility(ESlateVisibility::Visible);

        UE_LOG(LogTemp, Log, TEXT("Progress Bar initialized. Starting at: %.2f"), InitialPercent);
    }

    // Start the timer to update the progress bar gradually
    GetWorld()->GetTimerManager().SetTimer(EXPBarUpdateTimer, this, &UGame_Over_Widget::UpdateEXPBar, 0.01f, true);
}



void UGame_Over_Widget::OnEXPBarFillComplete()
{
    UE_LOG(LogTemp, Log, TEXT("EXP Bar fill complete!"));

    ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Ren)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to ARen_Low_Poly_Character."));
        return;
    }

    // Check for queued techniques and display notifications
    if (Ren->QueuedUnlockTechniques.Num() > 0)
    {
        // Copy the queue to avoid modifying it during iteration
        TArray<FString> TempQueue = Ren->QueuedUnlockTechniques;

        for (const FString& TechniqueName : TempQueue)
        {
            // Unlock the technique (assumes UnlockQueuedTechniques() processes all techniques)
            Ren->UnlockQueuedTechniques();

            // Show the notification for the unlocked technique
            FString NotificationMessage = FString::Printf(TEXT("Unlocked Technique: %s"), *TechniqueName);
            ShowNotification(NotificationMessage);

            // Log for debugging purposes
            UE_LOG(LogTemp, Log, TEXT("Notification shown: %s"), *NotificationMessage);
        }

        // Clear the original queue after processing
        Ren->QueuedUnlockTechniques.Empty();
    }
    else
    {
        // Log if no techniques were queued (optional)
        UE_LOG(LogTemp, Warning, TEXT("No techniques were queued for unlocking."));
    }

}



void UGame_Over_Widget::UpdateEXPUI()
{


    if (WeaponLevelText)
    {
        WeaponLevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), WeaponProficiency.WeaponLevel)));
        WeaponLevelText->SetVisibility(ESlateVisibility::Visible);
    }

    if (CurrentEXPText)
    {
        // Directly set the formatted text for CurrentEXP / EXPToNextLevel
        CurrentEXPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), WeaponProficiency.CurrentEXP, WeaponProficiency.EXPToNextLevel)));
        CurrentEXPText->SetVisibility(ESlateVisibility::Visible);
    }

   
}




void UGame_Over_Widget::HandleLevelUp()
{

    WeaponProficiency.WeaponLevel++; // Increment level
    WeaponProficiency.EXPToNextLevel *= 1.1f; // Example: Increase EXP requirement by 10%



}




void UGame_Over_Widget::ShowNotification(const FString& Message)
{
    if (NotificationText)
    {
        NotificationText->SetText(FText::FromString(Message));
        NotificationText->SetVisibility(ESlateVisibility::Visible);

        // Hide the notification after 3 seconds
        GetWorld()->GetTimerManager().SetTimer(NotificationHideTimer, this, &UGame_Over_Widget::ClearNotification, 3.0f, false);

        UE_LOG(LogTemp, Log, TEXT("Notification shown: %s"), *Message);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NotificationText is not valid."));
    }
}




void UGame_Over_Widget::ClearNotification()
{
    if (NotificationText)
    {
        NotificationText->SetVisibility(ESlateVisibility::Hidden);
        UE_LOG(LogTemp, Log, TEXT("Notification cleared."));
    }
}





void UGame_Over_Widget::UpdateEXPBar()
{

    if (RemainingEXPToAdd <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("No remaining EXP to add."));
        GetWorld()->GetTimerManager().ClearTimer(EXPBarUpdateTimer);
        OnEXPBarFillComplete();
        return;
    }

    // Calculate the amount of progress to add per tick
    float ProgressToAdd = FMath::Min(RemainingEXPToAdd * 0.01f, WeaponProficiency.EXPToNextLevel - WeaponProficiency.CurrentEXP);
    WeaponProficiency.CurrentEXP += ProgressToAdd;
    RemainingEXPToAdd -= ProgressToAdd;

    // Update the progress bar percentage
    if (EXPProgressBar)
    {
        float NewPercent = WeaponProficiency.CurrentEXP / WeaponProficiency.EXPToNextLevel;
        EXPProgressBar->SetPercent(NewPercent);

        UE_LOG(LogTemp, Log, TEXT("Progress Bar updated. Current percent: %.2f"), NewPercent);
    }

    // If the EXP reaches or exceeds the next level threshold
    if (WeaponProficiency.CurrentEXP >= WeaponProficiency.EXPToNextLevel)
    {
        // Handle level-up logic
        WeaponProficiency.CurrentEXP = 0.0f;
        WeaponProficiency.WeaponLevel++;
        WeaponProficiency.EXPToNextLevel *= 1.25f; // Example scaling for next level

        UE_LOG(LogTemp, Log, TEXT("Level up! New level: %d, Next EXP threshold: %.2f"), WeaponProficiency.WeaponLevel, WeaponProficiency.EXPToNextLevel);

        // Update UI for new level
        UpdateEXPUI();
    }

}





void UGame_Over_Widget::SetUpGameOverUI(int32 FinalScore, int32 HighScore)
{
    // Set the target score to the player's final score
    TargetScore = FinalScore;

    // Immediately set the high score
    if (HighScoreText)
    {
        HighScoreText->SetText(FText::FromString(FString::Printf(TEXT("High Score: %d"), HighScore)));
    }

    // Directly set CurrentDisplayedScore to TargetScore to display the final score immediately
   // CurrentDisplayedScore = TargetScore;

    // Update the UI to show the final score
    if (FinalScoreText)
    {
        FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), CurrentDisplayedScore)));
    }
}



void UGame_Over_Widget::UpdateDisplayedScore()
{
    // Increment the displayed score by ScoreStep
    CurrentDisplayedScore++;

    // Cap the displayed score at the target score
    if (CurrentDisplayedScore >= TargetScore)
    {
        CurrentDisplayedScore = TargetScore;

        // Stop the timer when the final score is reached
        GetWorld()->GetTimerManager().ClearTimer(ScoreUpdateTimer);

        PlayButtonsFadeInAnimation();

        ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

        if (Ren)
        {
            Ren->Score_Reaction_Anim();
            // Now that the animation is complete, update the high score
            Ren->UpdateHighScore(TargetScore);  // Update high score here

            UpdateEXPUI();

            UpdateEXPBar();

            StartEXPBarFill(RemainingEXPToAdd);

        }
    }

    // Update the final score text with the current displayed score
    if (FinalScoreText)
    {
        FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), CurrentDisplayedScore)));
    }

}



void UGame_Over_Widget::StartScoreAnimation()
{

    // Ensure the timer is cleared before starting a new animation
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ScoreUpdateTimer);

        // Start a timer to increment the displayed score
        GetWorld()->GetTimerManager().SetTimer(
            ScoreUpdateTimer,
            this,
            &UGame_Over_Widget::UpdateDisplayedScore,
            0.03f,
            true // Loop the timer
        );
    }


}
