// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Over_Widget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/BackgroundBlur.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Results_camera.h"
#include "LowPoly_Survival_GameMode.h"
#include "Components/Image.h"
#include "Components/Button.h"





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
