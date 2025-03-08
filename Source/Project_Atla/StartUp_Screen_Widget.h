// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Animation/WidgetAnimation.h"
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
    // Widget References
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* WidgetSwitcher;

    UPROPERTY(meta = (BindWidget))
    UImage* NovaeInteractiveLogo;

    UPROPERTY(meta = (BindWidget))
    UImage* UnrealEngineLogo;

    // Your specific animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* CompanyLogoAnim;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* UnrealLogoAnim;

private:
    // Timer handles
    FTimerHandle CompanyLogoTimerHandle;
    FTimerHandle UnrealLogoTimerHandle;

    // Animation completion delegates
    FWidgetAnimationDynamicEvent CompanyLogoAnimFinishedDelegate;

    // Sequence methods
    void PlayCompanyLogoAnimation();
    void ReverseCompanyLogoAnimation();
    void OnCompanyLogoAnimationComplete();
    void PlayUnrealLogoAnimation();
    void ReverseUnrealLogoAnimation();
    void OpenMainMenu();
};
