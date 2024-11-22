// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Over_Widget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/BackgroundBlur.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Results_camera.h"





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
    // Start a timer that will call UpdateBlurEffect every 0.03 seconds
    GetWorld()->GetTimerManager().SetTimer(
        BlurAnimationTimer,
        this,
        &UGame_Over_Widget::UpdateBlurEffect,
        0.08f,   // Update every 0.03 seconds
        true ,
        4.0f// Loop the timer
    );
}


void UGame_Over_Widget::UpdateBlurEffect()
{
    if (BackgroundBlur && CurrentBlurStrength < MaxBlurStrength)
    {
        CurrentBlurStrength += (MaxBlurStrength / BlurDuration) * 0.08f;
        BackgroundBlur->SetBlurStrength(CurrentBlurStrength);
    }
    else
    {
        // Ensure the blur animation has stopped
        GetWorld()->GetTimerManager().ClearTimer(BlurAnimationTimer);

        // Log debug message to confirm the timer has ended
        UE_LOG(LogTemp, Log, TEXT("Blur Effect Complete."));

        // Check and play the animations
        if (GameOverTextAnimation)
        {
            UE_LOG(LogTemp, Log, TEXT("Playing GameOverTextAnimation."));
            PlayAnimation(GameOverTextAnimation);
            Game_Over_Text->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("GameOverTextAnimation is null."));
        }
    }

}




void UGame_Over_Widget::OnGameOverTextAnimationComplete()
{
   
    StartCameraFade();



}



void UGame_Over_Widget::PlayGameOverAnimation()
{
    if (GameOverTextAnimation)
    {
        // Log when the animation is about to play
        UE_LOG(LogTemp, Log, TEXT("Playing GameOverTextAnimation"));

        // Play the animation
        PlayAnimation(GameOverTextAnimation);

        // Get the duration of the animation to set the timer
        float AnimationDuration = GameOverTextAnimation->GetEndTime();
        UE_LOG(LogTemp, Log, TEXT("GameOverTextAnimation Duration: %f"), AnimationDuration);


        GetWorld()->GetTimerManager().SetTimer(
            AnimationTimerHandle,
            this,
            &UGame_Over_Widget::OnGameOverTextAnimationComplete,
            AnimationDuration,
            false // Only trigger once

        );
    }
    

}

void UGame_Over_Widget::StartCameraFade()
{

    UE_LOG(LogTemp, Log, TEXT("Starting Camera Fade..."));

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        // Start the fade with a black color and fade in over 1 second
        PlayerController->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0f, 1.0f), 1.0f, true, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Player Controller is null!"));
    }


}

void UGame_Over_Widget::SwitchToNewCamera()
{



    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController && Results_Camera)
    {
        PlayerController->SetViewTargetWithBlend(Results_Camera, 1.0f); // Blend time of 1 second
    }

    // Proceed to scores fade-in animation
    StartScoreAnimation();



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
