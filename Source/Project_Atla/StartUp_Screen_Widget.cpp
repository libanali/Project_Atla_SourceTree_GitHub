// Fill out your copyright notice in the Description page of Project Settings.


#include "StartUp_Screen_Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Components/CanvasPanel.h"

void UStartUp_Screen_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    // Debug which components are missing
    if (!WidgetSwitcher)
    {
        UE_LOG(LogTemp, Error, TEXT("StartUp_Screen_Widget: WidgetSwitcher is missing!"));
    }
    if (!NovaeInteractiveCanvas)
    {
        UE_LOG(LogTemp, Error, TEXT("StartUp_Screen_Widget: NovaeInteractiveCanvas is missing!"));
    }
    if (!UnrealEngineCanvas)
    {
        UE_LOG(LogTemp, Error, TEXT("StartUp_Screen_Widget: UnrealEngineCanvas is missing!"));
    }

    // Debug which animations are missing
    if (!NovaeCanvasAnim)
    {
        UE_LOG(LogTemp, Error, TEXT("StartUp_Screen_Widget: NovaeCanvasAnim is missing!"));
    }
    if (!UnrealEngineCanvasAnim)
    {
        UE_LOG(LogTemp, Error, TEXT("StartUp_Screen_Widget: UnrealEngineCanvasAnim is missing!"));
    }

    // Continue only if required components are present
    if (!WidgetSwitcher || !NovaeInteractiveCanvas || !UnrealEngineCanvas)
    {
        UE_LOG(LogTemp, Error, TEXT("StartUp_Screen_Widget is missing required components! Sequence will not start."));
        return;
    }

    // Initially set the widget switcher to show the company logo
    WidgetSwitcher->SetActiveWidgetIndex(0);

    UE_LOG(LogTemp, Warning, TEXT("Starting logo sequence"));

    // Bind animation completion delegates
    if (NovaeCanvasAnim)
    {
        CompanyLogoAnimFinishedDelegate.BindDynamic(this, &UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete);
        BindToAnimationFinished(NovaeCanvasAnim, CompanyLogoAnimFinishedDelegate);
        UE_LOG(LogTemp, Warning, TEXT("NovaeCanvasAnim delegate bound successfully"));
    }

    if (UnrealEngineCanvasAnim)
    {
        UnrealLogoAnimFinishedDelegate.BindDynamic(this, &UStartUp_Screen_Widget::OnUnrealLogoAnimFinished);
        BindToAnimationFinished(UnrealEngineCanvasAnim, UnrealLogoAnimFinishedDelegate);
        UE_LOG(LogTemp, Warning, TEXT("UnrealEngineCanvasAnim delegate bound successfully"));
    }

    // Start the sequence
    PlayCompanyLogoAnimation();

}


void UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete()
{

    // Skip if reverse animation is already in progress
    if (bCompanyReverseInProgress)
        return;


    // Clear the timer in case this was called by the animation delegate
    GetWorld()->GetTimerManager().ClearTimer(CompanyLogoTimerHandle);

    UE_LOG(LogTemp, Warning, TEXT("Novae logo animation completed, waiting 3 seconds"));

    // Step 2: Wait 3 seconds after forward animation completes
    GetWorld()->GetTimerManager().SetTimer(CompanyLogoTimerHandle,
        this, &UStartUp_Screen_Widget::ReverseCompanyLogoAnimation, 3.0f, false);
}



void UStartUp_Screen_Widget::OnUnrealLogoAnimFinished()
{
    // Clear any existing timers
    GetWorld()->GetTimerManager().ClearTimer(UnrealLogoTimerHandle);

    UE_LOG(LogTemp, Warning, TEXT("Unreal logo animation completed, waiting 3 seconds"));

    // Step 6: Wait 3 seconds after forward animation completes
    GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle,
        this, &UStartUp_Screen_Widget::ReverseUnrealLogoAnimation, 3.0f, false);
}



void UStartUp_Screen_Widget::PlayCompanyLogoAnimation()
{
    // Make sure we're showing the Novae Interactive Canvas
    WidgetSwitcher->SetActiveWidgetIndex(0);

    UE_LOG(LogTemp, Warning, TEXT("Playing Novae Interactive Canvas Logo animation forward"));

    // Play the company logo animation
    if (NovaeCanvasAnim)
    {
        PlayAnimation(NovaeCanvasAnim);

        // If binding to animation finished doesn't work, use a timer as fallback
        float AnimDuration = NovaeCanvasAnim->GetEndTime();
        GetWorld()->GetTimerManager().SetTimer(CompanyLogoTimerHandle, this, &UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete, AnimDuration, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NovaeCanvasAnim not found!"));
        // Wait 3 seconds then move to next step
        GetWorld()->GetTimerManager().SetTimer(CompanyLogoTimerHandle, this, &UStartUp_Screen_Widget::OnCompanyLogoAnimationComplete, 3.0f, false);
    }
}



void UStartUp_Screen_Widget::ReverseCompanyLogoAnimation()
{

    // Prevent duplicate calls
    if (bCompanyReverseInProgress)
        return;

    bCompanyReverseInProgress = true;

    UE_LOG(LogTemp, Warning, TEXT("Playing Novae Interactive Canvas Logo animation in reverse"));

    // Play the company logo animation in reverse (fade out)
    if (NovaeCanvasAnim)
    {
        // Play the animation in reverse
        PlayAnimation(NovaeCanvasAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);

        // Clear any existing timers first
        GetWorld()->GetTimerManager().ClearTimer(CompanyLogoTimerHandle);

        // Wait for the reverse animation to complete
        float AnimDuration = NovaeCanvasAnim->GetEndTime();
        UE_LOG(LogTemp, Warning, TEXT("Setting timer for WaitAfterNovaeReverse: %f seconds"), AnimDuration);
        GetWorld()->GetTimerManager().SetTimer(CompanyLogoTimerHandle, this, &UStartUp_Screen_Widget::WaitAfterNovaeReverse, AnimDuration + 0.1f, false);
    }
    else
    {
        // Skip to waiting after Novae reverse
        WaitAfterNovaeReverse();
    }

}



void UStartUp_Screen_Widget::WaitAfterNovaeReverse()
{
    // Reset flag as we're done with company logo sequence
    bCompanyReverseInProgress = false;

    UE_LOG(LogTemp, Warning, TEXT("Novae reverse animation complete, waiting 1 second"));

    // Clear existing timers to be safe
    GetWorld()->GetTimerManager().ClearTimer(CompanyLogoTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(UnrealLogoTimerHandle);

    UE_LOG(LogTemp, Warning, TEXT("Setting timer for PlayUnrealLogoAnimation: 1.0 second"));

    // Use a lambda to ensure direct call and add extensive logging
    FTimerDelegate TimerDel;
    TimerDel.BindLambda([this]() {
        UE_LOG(LogTemp, Warning, TEXT("DIRECT LAMBDA CALLBACK - About to call PlayUnrealLogoAnimation"));
        PlayUnrealLogoAnimation();
        });

    GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, TimerDel, 1.0f, false);
}



void UStartUp_Screen_Widget::PlayUnrealLogoAnimation()
{
    UE_LOG(LogTemp, Warning, TEXT("PlayUnrealLogoAnimation ENTERED"));

    // Clear both timers to be absolutely sure
    GetWorld()->GetTimerManager().ClearTimer(CompanyLogoTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(UnrealLogoTimerHandle);

    UE_LOG(LogTemp, Warning, TEXT("*** TRANSITIONING TO UNREAL ANIMATION ***"));

    // Switch to Unreal Engine logo
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(1);
        UE_LOG(LogTemp, Warning, TEXT("WidgetSwitcher set to index 1"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WidgetSwitcher is NULL!"));
    }

    UE_LOG(LogTemp, Warning, TEXT("Playing Unreal Engine Canvas Logo animation forward"));

    // Play the Unreal Engine logo animation
    if (UnrealEngineCanvasAnim)
    {
        PlayAnimation(UnrealEngineCanvasAnim);
        UE_LOG(LogTemp, Warning, TEXT("UnrealEngineCanvasAnim started playing"));

        // Wait for animation to complete
        float AnimDuration = UnrealEngineCanvasAnim->GetEndTime();
        UE_LOG(LogTemp, Warning, TEXT("UnrealEngineCanvasAnim duration: %f seconds"), AnimDuration);
        GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, this, &UStartUp_Screen_Widget::OnUnrealLogoAnimFinished, AnimDuration + 0.1f, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealEngineCanvasAnim not found!"));
        // Wait 3 seconds then continue sequence
        GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, this, &UStartUp_Screen_Widget::OnUnrealLogoAnimFinished, 3.0f, false);
    }

}



void UStartUp_Screen_Widget::ReverseUnrealLogoAnimation()
{
    // Prevent duplicate calls
    if (bUnrealReverseInProgress)
        return;

    bUnrealReverseInProgress = true;

    UE_LOG(LogTemp, Warning, TEXT("Playing Unreal Engine Canvas Logo animation in reverse"));

    // IMPORTANT: Unbind the forward animation delegate before playing in reverse
    if (UnrealEngineCanvasAnim)
    {
        UnbindAllFromAnimationFinished(UnrealEngineCanvasAnim);

        // Play the animation in reverse
        PlayAnimation(UnrealEngineCanvasAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);

        // We'll rely solely on the timer now since we unbound the delegate
        float AnimDuration = UnrealEngineCanvasAnim->GetEndTime();
        GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, this,
            &UStartUp_Screen_Widget::WaitAfterUnrealReverse, AnimDuration + 0.2f, false);

        UE_LOG(LogTemp, Warning, TEXT("Set timer for WaitAfterUnrealReverse: %f seconds"),
            AnimDuration + 0.2f);
    }
    else
    {
        // Skip to waiting after Unreal reverse
        WaitAfterUnrealReverse();
    }
}


void UStartUp_Screen_Widget::WaitAfterUnrealReverse()
{

    // Reset flag as we're done with unreal logo sequence
    bUnrealReverseInProgress = false;

    UE_LOG(LogTemp, Warning, TEXT("Unreal reverse animation complete, waiting 1 second"));

    // Clear any existing timers to be safe
    GetWorld()->GetTimerManager().ClearTimer(UnrealLogoTimerHandle);

    // Use a lambda to ensure direct call and add extensive logging
    FTimerDelegate TimerDel;
    TimerDel.BindLambda([this]() {
        UE_LOG(LogTemp, Warning, TEXT("DIRECT LAMBDA CALLBACK - About to call OpenMainMenu"));
        OpenMainMenu();
        });

    GetWorld()->GetTimerManager().SetTimer(UnrealLogoTimerHandle, TimerDel, 1.0f, false);
}



void UStartUp_Screen_Widget::OpenMainMenu()
{

    UE_LOG(LogTemp, Warning, TEXT("OpenMainMenu ENTERED"));

    // Add this line to debug if the method is being called multiple times
    static int callCount = 0;
    UE_LOG(LogTemp, Warning, TEXT("OpenMainMenu called %d times"), ++callCount);

    // Add check for valid world
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to open main menu: GetWorld() returned null"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Opening main menu level"));

    // Try alternative method if the standard one isn't working
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("Using PlayerController to open level"));
        PC->ClientTravel("Main_Menu_Level", ETravelType::TRAVEL_Absolute);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Using standard OpenLevel method"));
        UGameplayStatics::OpenLevel(World, FName("Main_Menu_Level"));
    }
}
