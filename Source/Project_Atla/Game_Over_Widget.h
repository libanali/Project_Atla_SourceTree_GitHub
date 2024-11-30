// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Weapon_Proficiency_Struct.h"
#include "Game_Over_Widget.generated.h"

/**
 * 
 */

class AResults_camera;

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

    UFUNCTION(BlueprintCallable)
    void StartScoreAnimation();

    void StartBlurEffect();

    void UpdateBlurEffect();

    void OnGameOverTextAnimationComplete();

    void PlayGameOverAnimation();

    void StartCameraFade();

    void SwitchToResultsCamera();

    void PlayScoresFadeInAnimation();

    void PlayResultsTitleFadeInAnimation();

    void PlayRenderImageFadeInAnimation();

    void PlayButtonsFadeInAnimation();

    void SetInputModeToUI();

   // void UpdateEXPBar();

   // void StartEXPBarFill(float AddedEXP);

   // void OnEXPBarFillComplete();

 //   void UpdateEXPUI();

    void ShowNotification(const FString& Message);

    void ClearNotification();

    void StartEXPTransferAnimation();

    void UpdateEXPAnimation();

    void OnQueuedEXPAdded();




    // Background blur widget
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UBackgroundBlur* BackgroundBlur;
    // UI elements
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* FinalScoreText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* HighScoreText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Game_Over_Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Results_Title_Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UImage* Render_Image;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* Retry_Button;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* MainMenu_Button;

    // Progress Bar for EXP
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
       class UProgressBar* EXPProgressBar;  // The progress bar for EXP

    // Text for displaying current weapon level
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* WeaponLevelText;  // Text block to display current weapon level

    // Text for displaying current EXP
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* CurrentEXPText;  // Text block to display current EXP

    // Text for displaying current EXP
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* QueuedEXPText;  // Text block to display current EXP

    // Text for displaying current EXP
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* EXPToNextLevelText;  // Text block to display current EXP
  
    // Text block for notifications (e.g., "Level Up!" or "EXP +50")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* NotificationText;

    // Text block for notifications (e.g., "Level Up!" or "EXP +50")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UVerticalBox* NotificationBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        FWeapon_Proficiency_Struct WeaponProficiency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notifications")
        int32 MaxNotifications = 5;

    // Final score to display
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 TargetScore;

    // Current displayed score
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 CurrentDisplayedScore;

    // Current blur strength (starts at 0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float CurrentBlurStrength = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
        AActor* Results_Camera;


protected:
    // Override NativeConstruct to initialize the widget
    virtual void NativeConstruct() override;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* GameOverTextAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* ScoreFadeInAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* Results_Title_Animation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* Render_Image_Animation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* Buttons_Animation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* NotificationAnimation;


private:



    // Maximum blur strength
    float MaxBlurStrength = 10.0f;

    // Duration of the blur effect (in seconds)
    float BlurDuration = 1.5f;

    float RemainingEXPToAdd;

    float FillSpeed;

    // Timer handle for updating the score display
    FTimerHandle ScoreUpdateTimer;

    // Timer handle for updating blur effect
    FTimerHandle BlurAnimationTimer;

    FTimerHandle AnimationTimerHandle;

    FTimerHandle RenderImageFadeInAnimationTimerHandle;

    FTimerHandle ResultsTitleFadeInAnimationTimerHandle;

    FTimerHandle ButtonsAnimationTimerHandle;

    FTimerHandle CameraFadeTimer;

    FTimerHandle ExpGainTimerHandle;

    FTimerHandle ExpBarUpdateTimer;

    FTimerHandle RewardNotificationTimer;

    FTimerHandle EXPBarUpdateTimer;

    FTimerHandle NotificationHideTimer;

    FTimerHandle NotificationClearTimer;

    FTimerHandle EXPUpdateTimerHandle;




    bool bIsGameOverTextAnimationComplete = false;
    bool bIsScoreFadeInComplete = false;
    bool bIsBlurAnimationComplete = false;

    float GameOverTextAnimationDuration = 1.0f;
    float ScoreFadeInDuration = 1.0f;


    float CurrentEXP;
    float QueuedEXP;
    float EXPToNextLevel;


    float UpdateInterval;

    float QueuedEXPIncrement;
};
