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

    // Function called every time the timer ticks to update the score
    UFUNCTION()
        void UpdateDisplayedScore();


    void StartScoreAnimation();



    // Final score to display
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 TargetScore;

    // Current displayed score
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 CurrentDisplayedScore;

protected:
    // Override NativeConstruct to initialize the widget
    virtual void NativeConstruct() override;


private:
    // UI elements
    UPROPERTY(meta = (BindWidget))
        class UTextBlock* FinalScoreText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* HighScoreText;

 

    // Timer handle for updating the score display
    FTimerHandle ScoreUpdateTimer;


};
