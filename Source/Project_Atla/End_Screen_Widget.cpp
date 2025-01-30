// Fill out your copyright notice in the Description page of Project Settings.


#include "End_Screen_Widget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/BackgroundBlur.h"
#include "Animation/WidgetAnimation.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/ScriptDelegates.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/Gameplaystatics.h"






void UEnd_Screen_Widget::NativeConstruct()
{

	Super::NativeConstruct();

    // Initialize panels
    if (ResultsPanel)
        ResultsPanel->SetVisibility(ESlateVisibility::Visible);

    if (StatsPanel)
        StatsPanel->SetVisibility(ESlateVisibility::Hidden);

    // Setup button bindings
    if (RetryButton)
        RetryButton->OnClicked.AddDynamic(this, &UEnd_Screen_Widget::OnRetryClicked);

    if (MainMenuButton)
        MainMenuButton->OnClicked.AddDynamic(this, &UEnd_Screen_Widget::OnMainMenuClicked);

    // Initialize blur
    if (BackgroundBlur)
        BackgroundBlur->SetBlurStrength(0.0f);

 

    // Make widget focusable
    SetIsFocusable(true);


    BindToAnimationFinished(BlurAnimation, OnAnimationFinishedEvent);



    // Debug widget components
    UE_LOG(LogTemp, Warning, TEXT("NativeConstruct - Checking Components:"));
    UE_LOG(LogTemp, Warning, TEXT("MainSwitcher valid: %d"), IsValid(MainSwitcher));
    UE_LOG(LogTemp, Warning, TEXT("ResultsPanel valid: %d"), IsValid(ResultsPanel));
    UE_LOG(LogTemp, Warning, TEXT("StatsPanel valid: %d"), IsValid(StatsPanel));
    UE_LOG(LogTemp, Warning, TEXT("StatsPanelAnimation valid: %d"), IsValid(StatsPanelAnimation));

    // Initialize panels
    if (ResultsPanel)
        ResultsPanel->SetVisibility(ESlateVisibility::Visible);

    if (StatsPanel)
    {
        StatsPanel->SetVisibility(ESlateVisibility::Hidden);
        UE_LOG(LogTemp, Warning, TEXT("Set StatsPanel to Hidden"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StatsPanel is null in NativeConstruct"));
    }

}




void UEnd_Screen_Widget::SetupGameOver(int32 FinalScore, int32 HighScore, int32 RoundNumber)
{

    // Update score texts
    if (FinalScoreText)
        FinalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FinalScore)));

    if (HighScoreText)
        HighScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), HighScore)));

    if (RoundText)
        RoundText->SetText(FText::FromString(FString::Printf(TEXT("%d"), RoundNumber)));


    // Start the game over sequence
    if (BlurAnimation)
    {
        OnAnimationFinishedEvent.BindDynamic(this, &UEnd_Screen_Widget::OnBlurComplete);
        BindToAnimationFinished(BlurAnimation, OnAnimationFinishedEvent);
        PlayAnimation(BlurAnimation);
    }

}

void UEnd_Screen_Widget::UpdateStats(float Attack, float Defense, float Elemental, float Health, float NewAttack, float NewDefense, float NewElemental, float NewHealth)
{

    // Update all stat texts with the appropriate format
    if (AttackText)
        AttackText->SetText(FormatStatText(TEXT("Attack"), Attack, NewAttack));

    if (DefenseText)
        DefenseText->SetText(FormatStatText(TEXT("Defense"), Defense, NewDefense));

    if (ElementalText)
        ElementalText->SetText(FormatStatText(TEXT("Elemental"), Elemental, NewElemental));

    if (HealthText)
        HealthText->SetText(FormatStatText(TEXT("Health"), Health, NewHealth));


}




void UEnd_Screen_Widget::UpdateExpProgress(int32 CurrentLevel, int32 NextLevel, float Progress)
{


    if (ExpProgressBar)
        ExpProgressBar->SetPercent(Progress);

    if (CurrentLevelText)
        CurrentLevelText->SetText(FText::AsNumber(CurrentLevel));

    if (NextLevelText)
        NextLevelText->SetText(FText::AsNumber(NextLevel));


}


void UEnd_Screen_Widget::SetWeaponType(const FString& WeaponName)
{

    if (CurrentWeaponText)
    {
        CurrentWeaponText->SetText(FText::FromString(WeaponName));
    }


}




void UEnd_Screen_Widget::SetResultsCamera(AResults_camera* Camera)
{

    Results_Camera = Camera;

}



/*
void UEnd_Screen_Widget::SetCharacterImage(EWeaponType WeaponType)
{

    UTexture2D* CharacterTexture = (WeaponType == EWeaponType::Sword) ? SwordCharacterTexture : StaffCharacterTexture;

    if (CharacterTexture)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(CharacterTexture);

        if (CharacterImage)
            CharacterImage->SetBrush(Brush);

        if (CharacterImage2)
            CharacterImage2->SetBrush(Brush);
    }

}

*/


void UEnd_Screen_Widget::OnRetryClicked()
{


    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }

    UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));


}




void UEnd_Screen_Widget::OnMainMenuClicked()
{


    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }

    UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));

}




void UEnd_Screen_Widget::OnBlurComplete()
{

    if (GameOverAnimation)
    {
        OnAnimationFinishedEvent.BindDynamic(this, &UEnd_Screen_Widget::OnGameOverTextComplete);
        BindToAnimationFinished(GameOverAnimation, OnAnimationFinishedEvent);
        PlayAnimation(GameOverAnimation);
    }


}

void UEnd_Screen_Widget::OnGameOverTextComplete()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        // Fade to black over 1 second
        PlayerController->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0f, 1.0f), 1.0f, true, true);

        // Set a timer to handle the camera transition after the fade
        FTimerHandle TransitionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TransitionTimerHandle,
            [this]()
            {
                SwitchToResultsCamera();
            },
            1.0f, // Wait for fade to complete
                false
                );
    }
}



void UEnd_Screen_Widget::SwitchToResultsCamera()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController && Results_Camera)
    {
        // Switch to Results Camera
        PlayerController->SetViewTargetWithBlend(
            Results_Camera,
            0.0f,  // Instant switch since we're faded to black
            EViewTargetBlendFunction::VTBlend_Linear
        );

        // Start fading from black
        PlayerController->ClientSetCameraFade(false, FColor::Black, FVector2D(1.0f, 0.0f), 1.0f, true, true);

        // After fade is complete, show results
        FTimerHandle ResultsTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            ResultsTimerHandle,
            [this]()
            {
                ShowPage(EGameOverPage::Results);
            },
            1.0f, // Wait for fade to complete
                false
                );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Results_Camera or PlayerController is null in SwitchToResultsCamera"));
    }
}



void UEnd_Screen_Widget::OnResultsPanelComplete()
{
    if (!MainSwitcher)
    {
        UE_LOG(LogTemp, Error, TEXT("MainSwitcher is null in OnResultsPanelComplete!"));
        return;
    }

    switch (PageEnum)
    {
    case EGameOverPage::Results:
    {
        // Create a named timer handle
        FTimerHandle StatsTransitionTimerHandle;

        // After Results page animation, wait and then transition to Stats
        GetWorld()->GetTimerManager().SetTimer(
            StatsTransitionTimerHandle,  // Pass timer handle by reference
            [this]()
            {
                ShowPage(EGameOverPage::Stats);
            },
            3.0f, // 3-second delay before showing Stats page
                false  // Do not loop
                );
        break;
    }
    case EGameOverPage::Stats:
        // Set up input mode for Stats page
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            FInputModeUIOnly InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            if (RetryButton)
            {
                RetryButton->SetKeyboardFocus();
            }
        }
        break;
    }
}





void UEnd_Screen_Widget::ShowPage(EGameOverPage Page)
{
    // Always update PageEnum
    PageEnum = Page;

    if (!MainSwitcher)
    {
        UE_LOG(LogTemp, Error, TEXT("MainSwitcher is null in ShowPage!"));
        return;
    }

    switch (Page)
    {
    case EGameOverPage::Results:
        // Ensure Results page is active
        MainSwitcher->SetActiveWidgetIndex(0);

        if (ResultsPanelAnimation)
        {
            // Bind completion event
            OnAnimationFinishedEvent.BindDynamic(this, &UEnd_Screen_Widget::OnResultsPanelComplete);
            BindToAnimationFinished(ResultsPanelAnimation, OnAnimationFinishedEvent);
            PlayAnimation(ResultsPanelAnimation);
        }
        break;

    case EGameOverPage::Stats:
        // Ensure Stats page is active
        MainSwitcher->SetActiveWidgetIndex(1);

        if (StatsPanel)
        {
            StatsPanel->SetVisibility(ESlateVisibility::Visible);
        }

        if (StatsPanelAnimation)
        {
            PlayAnimation(StatsPanelAnimation);
        }
        break;
    }

}




FText UEnd_Screen_Widget::FormatStatText(const FString& StatName, float CurrentValue, float NewValue)
{
    // If values are different, show the change
    if (FMath::Abs(CurrentValue - NewValue) > SMALL_NUMBER)
    {
        return FText::FromString(FString::Printf(TEXT("%d > %d"),
            *StatName, FMath::RoundToInt(CurrentValue), FMath::RoundToInt(NewValue)));
    }
    // Otherwise just show current value
    return FText::FromString(FString::Printf(TEXT("%d"),
        *StatName, FMath::RoundToInt(CurrentValue)));
}
