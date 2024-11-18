// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Over_Widget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UGame_Over_Widget::NativeConstruct()
{

	CurrentDisplayedScore = 0;

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

    // Start animating the displayed score
    if (FinalScoreText)
    {
        GetWorld()->GetTimerManager().SetTimer(
            ScoreUpdateTimer,
            this,
            &UGame_Over_Widget::UpdateDisplayedScore,
            0.02f,  // Tick every 0.02 seconds (adjust for speed)
            true
        );
    }
}

void UGame_Over_Widget::UpdateDisplayedScore()
{

    // Increment the displayed score
    CurrentDisplayedScore += FMath::Min(10, TargetScore - CurrentDisplayedScore); // Increment faster initially

    // Update the UI
    if (FinalScoreText)
    {
        FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), CurrentDisplayedScore)));
    }

    // Stop the timer when we reach the target score
    if (CurrentDisplayedScore >= TargetScore)
    {
        CurrentDisplayedScore = TargetScore;
        GetWorld()->GetTimerManager().ClearTimer(ScoreUpdateTimer);
    }


}
