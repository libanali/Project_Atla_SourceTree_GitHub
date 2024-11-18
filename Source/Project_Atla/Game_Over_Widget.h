// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game_Over_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UGame_Over_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // Function to set up the UI with the final score and high score
    UFUNCTION(BlueprintCallable, Category = "UI")
        void SetUpGameOverUI(int32 FinalScore, int32 HighScore);

protected:
    // Override NativeConstruct to initialize the widget
    virtual void NativeConstruct() override;

    // Function called every time the timer ticks to update the score
    UFUNCTION()
        void UpdateDisplayedScore();

private:
    // UI elements
    UPROPERTY(meta = (BindWidget))
        class UTextBlock* FinalScoreText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* HighScoreText;

    // Final score to display
    int32 TargetScore;

    // Current displayed score
    int32 CurrentDisplayedScore;

    // Timer handle for updating the score display
    FTimerHandle ScoreUpdateTimer;


};
