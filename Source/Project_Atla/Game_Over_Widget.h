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




    void StartBlurEffect();

    void UpdateBlurEffect();

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

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* GameOverText;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* GameOverTextAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* ScoreFadeInAnimation;

    // Background blur widget
    UPROPERTY(meta = (BindWidget))
       class UBackgroundBlur* BackgroundBlur;
 
    // Current blur strength (starts at 0)
    float CurrentBlurStrength = 0.0f;

    // Maximum blur strength
    float MaxBlurStrength = 10.0f;

    // Duration of the blur effect (in seconds)
    float BlurDuration = 1.5f;

    // Timer handle for updating the score display
    FTimerHandle ScoreUpdateTimer;

    // Timer handle for updating blur effect
    FTimerHandle BlurAnimationTimer;


    bool bIsGameOverTextAnimationComplete = false;
    bool bIsScoreFadeInComplete = false;
    bool bIsBlurAnimationComplete = false;

    float GameOverTextAnimationDuration = 1.0f;
    float ScoreFadeInDuration = 1.0f;

};
