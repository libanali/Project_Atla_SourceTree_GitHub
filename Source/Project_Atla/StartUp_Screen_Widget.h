// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartUp_Screen_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UStartUp_Screen_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
    // Called when widget is constructed
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UWidgetSwitcher* WidgetSwitcher;

    UPROPERTY(meta = (BindWidget))
    class UCanvasPanel* NovaeInteractiveCanvas;

    UPROPERTY(meta = (BindWidget))
    class UCanvasPanel* UnrealEngineCanvas;

    // Your specific animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    class UWidgetAnimation* NovaeCanvasAnim;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    class UWidgetAnimation* UnrealEngineCanvasAnim;

private:
    // Timer handles
    FTimerHandle CompanyLogoTimerHandle;
    FTimerHandle UnrealLogoTimerHandle;

    // Animation completion delegates
    FWidgetAnimationDynamicEvent CompanyLogoAnimFinishedDelegate;
    FWidgetAnimationDynamicEvent UnrealLogoAnimFinishedDelegate;

    // Sequence methods
    UFUNCTION()
    void OnCompanyLogoAnimationComplete();

    UFUNCTION()
    void OnUnrealLogoAnimFinished();

    void PlayCompanyLogoAnimation();
    void ReverseCompanyLogoAnimation();
    void WaitAfterNovaeReverse();  // New method for step 4
    void PlayUnrealLogoAnimation();
    void ReverseUnrealLogoAnimation();
    void WaitAfterUnrealReverse();  // New method for step 8
    void OpenMainMenu();

    bool bCompanyReverseInProgress = false;
    bool bUnrealReverseInProgress = false;
};
