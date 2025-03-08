// Fill out your copyright notice in the Description page of Project Settings.


#include "StartUp_Screen_Widget.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"

void UStartUp_Screen_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    // Make sure we have all required components
    if (!WidgetSwitcher || !NovaeInteractiveLogo || !UnrealEngineLogo)
    {
        UE_LOG(LogTemp, Error, TEXT("StartUp_Screen_Widget is missing required components!"));
        return;
    }

    // Initially set the widget switcher to show the company logo
    WidgetSwitcher->SetActiveWidgetIndex(0);

    // Bind animation completion delegate
    if (CompanyLogoAnim)
    {
        CompanyLogoAnimFinishedDelegate.BindDynamic(this, &UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete);
        BindToAnimationFinished(CompanyLogoAnim, CompanyLogoAnimFinishedDelegate);
    }

    // Start the sequence
    PlayCompanyLogoAnimation();

}



void UStartUp_Screen_Widget::PlayCompanyLogoAnimation()
{

    // Make sure we're showing the company logo
    WidgetSwitcher->SetActiveWidgetIndex(0);

    // Play the company logo animation
    if (CompanyLogoAnim)
    {
        PlayAnimation(CompanyLogoAnim);

        // If binding to animation finished doesn't work, use a timer as fallback
        float AnimDuration = CompanyLogoAnim->GetEndTime();
        GetWorld()->GetTimerManager().SetTimer(CompanyLogoTimerHandle, this, &UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete, AnimDuration + 3.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CompanyLogoAnim not found!"));
        // Wait 3 seconds then move to next step
        GetWorld()->GetTimerManager().SetTimer(CompanyLogoTimerHandle, this, &UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete, 3.0f, false);
    }
}



void UStartUp_Screen_Widget::ReverseCompanyLogoAnimation()
{

    // Play the company logo animation in reverse (fade out)
    if (CompanyLogoAnim)
    {
        // Play the animation in reverse
        PlayAnimation(CompanyLogoAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);

        // Wait for the reverse animation to complete before showing Unreal logo
        float AnimDuration = CompanyLogoAnim->GetEndTime();
        GetWorld()->GetTimerManager().SetTimer(CompanyLogoTimerHandle, this, &UStartUp_Screen_Widget::PlayUnrealLogoAnimation, AnimDuration, false);
    }
    else
    {
        // Skip to unreal logo
        PlayUnrealLogoAnimation();
    }





}



void UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete()
{

    // Clear the timer in case this was called by the animation delegate
    GetWorld()->GetTimerManager().ClearTimer(CompanyLogoTimerHandle);

    // Switch to Unreal logo and play its animation
    PlayUnrealLogoAnimation();


}





void UStartUp_Screen_Widget::PlayUnrealLogoAnimation()
{

    // Switch to Unreal Engine logo
    WidgetSwitcher->SetActiveWidgetIndex(1);

    // Play the Unreal Engine logo animation
    if (UnrealLogoAnim)
    {
        PlayAnimation(UnrealLogoAnim);

        // Wait for animation to complete + 3 seconds before opening main menu
        float AnimDuration = UnrealLogoAnim->GetEndTime();
        GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, this, &UStartUp_Screen_Widget::OpenMainMenu, AnimDuration + 3.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UnrealLogoAnim not found!"));
        // Wait 3 seconds then open main menu
        GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, this, &UStartUp_Screen_Widget::OpenMainMenu, 3.0f, false);
    }



}



void UStartUp_Screen_Widget::ReverseUnrealLogoAnimation()
{

    // Play the Unreal Engine logo animation in reverse (fade out)
    if (UnrealLogoAnim)
    {
        // Play the animation in reverse
        PlayAnimation(UnrealLogoAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);

        // Wait for the reverse animation to complete before opening main menu
        float AnimDuration = UnrealLogoAnim->GetEndTime();
        GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, this, &UStartUp_Screen_Widget::OpenMainMenu, AnimDuration, false);
    }
    else
    {
        // Open main menu immediately
        OpenMainMenu();
    }



}



void UStartUp_Screen_Widget::OpenMainMenu()
{

    UGameplayStatics::OpenLevel(GetWorld(), FName("Main_Menu_Level"));

}
