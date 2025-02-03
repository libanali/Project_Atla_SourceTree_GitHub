// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Results_camera.h"
#include "Camera/CameraActor.h"
#include "GameEnums.h"
#include "End_Screen_Widget.generated.h"

/**
 * 
 */




UENUM()
enum class EGameOverPage : uint8
{
    None,
    Results,
    Stats
};



UCLASS()
class PROJECT_ATLA_API UEnd_Screen_Widget : public UUserWidget
{
	GENERATED_BODY()

public:


    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
        class UTextBlock* WeaponLevelText;


protected:
    virtual void NativeConstruct() override;

    // Main widget components
    UPROPERTY(meta = (BindWidget))
      class UWidgetSwitcher* MainSwitcher;

    UPROPERTY(meta = (BindWidget))
       class UBackgroundBlur* BackgroundBlur;

    // Game Over Text
    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* GameOverText;

    // Results Page Components
    UPROPERTY(meta = (BindWidget))
        class  UCanvasPanel* ResultsPanel;

    UPROPERTY(meta = (BindWidget))
        class  UImage* CharacterImage;

    UPROPERTY(meta = (BindWidget))
        class  UImage* CharacterImage2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Images")
       class UTexture2D* SwordCharacterTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Images")
       class UTexture2D* StaffCharacterTexture;

    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* FinalScoreText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* HighScoreText;

    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* RoundText;

    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* CurrentWeaponText;

    // Stats Page Components
    UPROPERTY(meta = (BindWidget))
        class  UCanvasPanel* StatsPanel;

    UPROPERTY(meta = (BindWidget))
        class  UProgressBar* ExpProgressBar;

    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* CurrentLevelText;

    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* NextLevelText;

    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* AttackText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* DefenseText;

    UPROPERTY(meta = (BindWidget))
        class  UTextBlock* ElementalText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* FireProfText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* IceProfText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* ThunderProfText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* EXPEarnedText;

    // Navigation Buttons
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* RetryButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class  UButton* MainMenuButton;

    UPROPERTY()
        AResults_camera* Results_Camera;

    // Animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* BlurAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* GameOverAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* ResultsPanelAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* StatsPanelAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* HighScoreAnimation;


public:
    // Initialize widget with results data
    void SetupGameOver(int32 FinalScore, int32 HighScore, int32 RoundNumber);

    // Update stats display
    void UpdateStats(float Attack, float Defense, float Elemental, float Health,
        float NewAttack, float NewDefense, float NewElemental, float NewHealth);

    // Update EXP progress
    void UpdateExpProgress(int32 CurrentLevel, int32 NextLevel, float Progress);

    // Set weapon type
    void SetWeaponType(const FString& WeaponName);

    // Set results camera
    void SetResultsCamera(AResults_camera* Camera);

    void SetWeaponLevel(int32 OldLevel, int32 NewLevel);

    void SetElementalLevels(int32 FireOldLevel, int32 FireNewLevel,
        int32 IceOldLevel, int32 IceNewLevel,
        int32 ThunderOldLevel, int32 ThunderNewLevel);

    void SetEXPEarned(float EXPAmount);

    void HandleHighScoreReveal(int32 StartValue, int32 EndValue);

    void SetCharacterImage(EWeaponType WeaponType);



protected:
    // Button callbacks
    UFUNCTION()
        void OnRetryClicked();

    UFUNCTION()
        void OnMainMenuClicked();

    // Animation complete handlers
    UFUNCTION()
        void OnBlurComplete();

    UFUNCTION()
        void OnGameOverTextComplete();

    UFUNCTION()
        void OnResultsPanelComplete();

    UFUNCTION()
        void SwitchToResultsCamera();

    // Helper functions
    void ShowPage(EGameOverPage Page);
    FText FormatStatText(const FString& StatName, float CurrentValue, float NewValue);


    UPROPERTY()
    FWidgetAnimationDynamicEvent OnAnimationFinishedEvent;

    EGameOverPage PageEnum;

    bool bIsNewHighScore;
    int32 CurrentDisplayedScore;
   // FTimerHandle ScoreUpdateTimerHandle;

private:


    // Animation sequence control
    int32 TargetRound;
    int32 CurrentDisplayedRound;
    int32 TargetScore;
    int32 TargetHighScore;
    bool bIsAnimatingRound;
    bool bIsAnimatingScore;
    bool bIsAnimatingHighScore;
    FTimerHandle RoundUpdateTimerHandle;
    FTimerHandle ScoreUpdateTimerHandle;
    FTimerHandle HighScoreUpdateTimerHandle;
    FTimerHandle StatsPageTimerHandle;

    // Animation functions
    void StartResultsSequence();
    void AnimateRoundCount();
    void AnimateScoreCount();
    void OnRoundAnimationComplete();
    void OnScoreAnimationComplete();
    void OnHighScoreAnimationComplete();
};
