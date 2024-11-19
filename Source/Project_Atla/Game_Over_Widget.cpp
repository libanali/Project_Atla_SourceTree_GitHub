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

    // Directly set CurrentDisplayedScore to TargetScore to display the final score immediately
    CurrentDisplayedScore = TargetScore;

    // Update the UI to show the final score
    if (FinalScoreText)
    {
        FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Final Score: %d"), CurrentDisplayedScore)));
    }
}

void UGame_Over_Widget::UpdateDisplayedScore()
{
 

}
