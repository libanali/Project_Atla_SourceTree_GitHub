// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
        class UTextBlock* WeaponLevelText;

    // Navigation Buttons
    UPROPERTY(meta = (BindWidget))
        class UButton* RetryButton;

    UPROPERTY(meta = (BindWidget))
        class  UButton* MainMenuButton;

    // Animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* BlurAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* GameOverAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* ResultsPanelAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        class  UWidgetAnimation* StatsPanelAnimation;

public:
    // Initialize widget with results data
    void SetupGameOver(int32 FinalScore, int32 HighScore, int32 RoundNumber);

    // Update stats display
    void UpdateStats(float Attack, float Defense, float Elemental, float Health,
        float NewAttack, float NewDefense, float NewElemental, float NewHealth);

    // Update EXP progress
    void UpdateExpProgress(int32 CurrentLevel, int32 NextLevel, float Progress);

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

    // Helper functions
    void ShowPage(EGameOverPage Page);
    FText FormatStatText(const FString& StatName, float CurrentValue, float NewValue);


	
};
