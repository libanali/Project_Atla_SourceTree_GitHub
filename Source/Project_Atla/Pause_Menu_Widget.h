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
    virtual void NativeDestruct() override;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UWidgetSwitcher* MenuSwitcher;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* ResumeButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* QuitButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UCanvasPanel* MainPauseCanvas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UCanvasPanel* ConfirmQuitCanvas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* ConfirmYesButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* ConfirmNoButton;

    UPROPERTY()
        UButton* LastFocusedButton;

    // Animation properties
    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* PauseMenuAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* QuitConfirmAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
        USoundBase* BackSound;

    UFUNCTION()
        void OnResumeClicked();

    UFUNCTION()
        void OnQuitClicked();

    UFUNCTION()
        void OnConfirmQuitYes();

    UFUNCTION()
        void OnConfirmQuitNo();

    UFUNCTION()
        void PlayBackSound();


    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;


public:
    void ShowPauseMenu();
    void HidePauseMenu();
    void HandleGoBack();
    void UpdateMenuState(int32 ActiveIndex);


};
