// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Demo_End_Screen_Widget.generated.h"

/**
 * 
 */

class UButton;
class UWidgetAnimation;
class UBackgroundBlur;
class UTextBlock;
class UImage;
class UBorder;

UCLASS()
class PROJECT_ATLA_API UDemo_End_Screen_Widget : public UUserWidget
{
	GENERATED_BODY()
	

public:


	virtual void NativeConstruct() override;


    void SetupDemoEnd();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	 UButton* Main_Menu_Button;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	 UButton* Steam_Store_Button;


     UPROPERTY(meta = (BindWidget))
     UBackgroundBlur* BackgroundBlur;

     UPROPERTY(meta = (BindWidget))
     UTextBlock* DemoCompleteText;

     UPROPERTY(meta = (BindWidget))
     UImage* DemoScreenImage;

     UPROPERTY(meta = (BindWidget))
     UBorder* BlackBorder;

     // Animations
     UPROPERTY(meta = (BindWidgetAnim), Transient)
     UWidgetAnimation* BlurAnimation;

     UPROPERTY(meta = (BindWidgetAnim), Transient)
     UWidgetAnimation* DemoCompleteAnimation;

     UPROPERTY(meta = (BindWidgetAnim), Transient)
     UWidgetAnimation* ButtonsRevealAnimation;


     UPROPERTY(meta = (BindWidgetAnim), Transient)
     UWidgetAnimation* DemoScreenImageAnimation;

     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
     USoundBase* NavigationSound;

     // Button callbacks
     UFUNCTION()
     void OnMainMenuClicked();

     UFUNCTION()
     void OnSteamStoreClicked();

     // Animation callbacks
     UFUNCTION()
     void OnBlurComplete();

     UFUNCTION()
     void OnDemoTextComplete();

     UFUNCTION()
     void OnButtonsRevealComplete();

private:
    // Animation finished delegate
    FWidgetAnimationDynamicEvent OnAnimationFinishedEvent;


    // Timer handles
    FTimerHandle TransitionTimerHandle;
    FTimerHandle ButtonsTimerHandle;


    void ShowButtons();
    void PlayNavigationSound();


protected:

    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
};
