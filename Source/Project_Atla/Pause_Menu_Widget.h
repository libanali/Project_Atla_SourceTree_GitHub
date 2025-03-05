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
 //   virtual void NativeDestruct() override;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UWidgetSwitcher* MenuSwitcher;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* ResumeButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* QuitButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* YesButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* NoButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UCanvasPanel* MainPauseCanvas;

    UPROPERTY(meta = (BindWidget))
        class UCanvasPanel* ConfirmationCanvas;

    UPROPERTY()
        UButton* LastFocusedButton;

    // Animation properties
    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* PauseMenuAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* ConfirmationMenuAnimation;

 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
        USoundBase* BackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
        USoundBase* NavigationSound;

    UFUNCTION()
        void OnResumeClicked();

    UFUNCTION()
        void PlayBackSound();

    UFUNCTION()
        void OnQuitClicked();

    UFUNCTION()
        void OnYesClicked();

    UFUNCTION()
        void OnNoClicked();


    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    int32 CurrentMenuState = 0;
public:
    void BindDelegates();
    void UnbindDelegates();
    void ShowPauseMenu();
    void HidePauseMenu();
    void HandleGoBack();
    void UpdateMenuState(int32 ActiveIndex);
    void PlayNavigationSound();


};
