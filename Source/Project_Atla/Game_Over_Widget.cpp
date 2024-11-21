// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Over_Widget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/BackgroundBlur.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"




void UGame_Over_Widget::NativeConstruct()
{

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
        0.03f,   // Update every 0.03 seconds
        true     // Loop the timer
    );
}


void UGame_Over_Widget::UpdateBlurEffect()
{

    // Calculate the smooth increment of blur strength using Lerp
    if (BackgroundBlur && CurrentBlurStrength < MaxBlurStrength)
    {
        // Interpolate between 0 and MaxBlurStrength over time
        CurrentBlurStrength = FMath::Lerp(CurrentBlurStrength, MaxBlurStrength, 0.1f); // 0.1f is the lerp factor

        // Apply the new blur strength
        BackgroundBlur->SetBlurStrength(CurrentBlurStrength);
    }
    else
    {
        // Stop the timer once we reach the max blur strength
        GetWorld()->GetTimerManager().ClearTimer(BlurAnimationTimer);
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
