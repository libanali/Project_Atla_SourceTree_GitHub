// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Pause_Menu_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UPause_Menu_Widget : public UUserWidget
{
	GENERATED_BODY()


protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
        class UWidgetSwitcher* MenuSwitcher;

    UPROPERTY(meta = (BindWidget))
        class UButton* ResumeButton;

    UPROPERTY(meta = (BindWidget))
        class UButton* QuitButton;

    UPROPERTY(meta = (BindWidget))
        class UCanvasPanel* MainPauseCanvas;

    UPROPERTY(meta = (BindWidget))
        class UCanvasPanel* ConfirmQuitCanvas;

    UPROPERTY(meta = (BindWidget))
        class UButton* ConfirmYesButton;

    UPROPERTY(meta = (BindWidget))
        class UButton* ConfirmNoButton;

    // Animation properties
    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* MainMenuAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* ConfirmMenuAnimation;

    UFUNCTION()
        void OnResumeClicked();

    UFUNCTION()
        void OnQuitClicked();

    UFUNCTION()
        void OnConfirmQuitYes();

    UFUNCTION()
        void OnConfirmQuitNo();

public:
    void ShowPauseMenu();
    void HidePauseMenu();
	
};
