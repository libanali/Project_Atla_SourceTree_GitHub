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

    UE_LOG(LogTemp, Warning, TEXT("========== STEP 1: Widget Construction =========="));
    UE_LOG(LogTemp, Warning, TEXT("MainSwitcher valid: %d"), IsValid(MainSwitcher));
    UE_LOG(LogTemp, Warning, TEXT("ResultsPanel valid: %d"), IsValid(ResultsPanel));
    UE_LOG(LogTemp, Warning, TEXT("StatsPanel valid: %d"), IsValid(StatsPanel));
    UE_LOG(LogTemp, Warning, TEXT("BlurAnimation valid: %d"), IsValid(BlurAnimation));
    UE_LOG(LogTemp, Warning, TEXT("GameOverAnimation valid: %d"), IsValid(GameOverAnimation));
    UE_LOG(LogTemp, Warning, TEXT("ResultsPanelAnimation valid: %d"), IsValid(ResultsPanelAnimation));
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

}




void UEnd_Screen_Widget::SetupGameOver(int32 FinalScore, int32 HighScore, int32 RoundNumber)
{
    UE_LOG(LogTemp, Warning, TEXT("========== STEP 2: Setting Up Game Over =========="));
    UE_LOG(LogTemp, Warning, TEXT("Final Score: %d, High Score: %d, Round: %d"),
        FinalScore, HighScore, RoundNumber);


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
        UE_LOG(LogTemp, Warning, TEXT("Starting Blur Animation"));
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
    UE_LOG(LogTemp, Warning, TEXT("========== STEP 3: Blur Animation Complete =========="));

    if (GameOverAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting Game Over Text Animation"));
        OnAnimationFinishedEvent.BindDynamic(this, &UEnd_Screen_Widget::OnGameOverTextComplete);
        BindToAnimationFinished(GameOverAnimation, OnAnimationFinishedEvent);
        PlayAnimation(GameOverAnimation);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Game Over Animation is null!"));
    }

}

void UEnd_Screen_Widget::OnGameOverTextComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("========== STEP 4: Game Over Text Complete =========="));
    UE_LOG(LogTemp, Warning, TEXT("Starting Camera Fade Sequence"));

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerController->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0f, 1.0f), 1.0f, true, true);
        UE_LOG(LogTemp, Warning, TEXT("Started fade to black"));

        FTimerHandle TransitionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TransitionTimerHandle,
            [this]()
            {
                UE_LOG(LogTemp, Warning, TEXT("Fade complete, switching to results camera"));
                SwitchToResultsCamera();
            },
            3.0f,
                false
                );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null!"));
    }
}



void UEnd_Screen_Widget::SwitchToResultsCamera()
{
    UE_LOG(LogTemp, Warning, TEXT("========== STEP 5: Switching to Results Camera =========="));

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController && Results_Camera)
    {
        PlayerController->SetViewTargetWithBlend(
            Results_Camera,
            0.0f,
            EViewTargetBlendFunction::VTBlend_Linear
        );
        UE_LOG(LogTemp, Warning, TEXT("Camera switched to Results Camera"));

        PlayerController->ClientSetCameraFade(false, FColor::Black, FVector2D(1.0f, 0.0f), 1.0f, true, true);
        UE_LOG(LogTemp, Warning, TEXT("Started fade from black"));

        FTimerHandle ResultsTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            ResultsTimerHandle,
            [this]()
            {
                UE_LOG(LogTemp, Warning, TEXT("Fade complete, showing results page"));
                ShowPage(EGameOverPage::Results);
            },
            2.0f,
                false
                );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Results_Camera or PlayerController is null! Camera: %d, Controller: %d"),
            IsValid(Results_Camera), IsValid(PlayerController));
    }
}



void UEnd_Screen_Widget::OnResultsPanelComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("========== STEP 6: Results Panel Complete =========="));
    UE_LOG(LogTemp, Warning, TEXT("Starting timer for Stats Page"));

    FTimerHandle StatsPageTimer;
    GetWorld()->GetTimerManager().SetTimer(
        StatsPageTimer,
        [this]()
        {
            UE_LOG(LogTemp, Warning, TEXT("Timer complete, showing stats page"));
            ShowPage(EGameOverPage::Stats);
        },
        3.0f,
            false
            );
}





void UEnd_Screen_Widget::ShowPage(EGameOverPage Page)
{
    if (!MainSwitcher)
    {
        UE_LOG(LogTemp, Error, TEXT("ShowPage: MainSwitcher is null!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("========== Showing Page: %d =========="), static_cast<int32>(Page));

    switch (Page)
    {
    case EGameOverPage::Results:
        UE_LOG(LogTemp, Warning, TEXT("Showing Results Page"));
        MainSwitcher->SetActiveWidgetIndex(0);

        if (ResultsPanel)
            ResultsPanel->SetVisibility(ESlateVisibility::Visible);
        if (StatsPanel)
            StatsPanel->SetVisibility(ESlateVisibility::Hidden);

        if (ResultsPanelAnimation)
        {
            OnAnimationFinishedEvent.BindDynamic(this, &UEnd_Screen_Widget::OnResultsPanelComplete);
            BindToAnimationFinished(ResultsPanelAnimation, OnAnimationFinishedEvent);
            PlayAnimation(ResultsPanelAnimation);
        }
        break;

    case EGameOverPage::Stats:
        UE_LOG(LogTemp, Warning, TEXT("Showing Stats Page"));
        MainSwitcher->SetActiveWidgetIndex(1);

        if (ResultsPanel)
            ResultsPanel->SetVisibility(ESlateVisibility::Hidden);
        if (StatsPanel)
            StatsPanel->SetVisibility(ESlateVisibility::Visible);  // Make sure stats panel is visible

        if (StatsPanelAnimation)
        {
            PlayAnimation(StatsPanelAnimation);
        }

        if (RetryButton)
        {
            RetryButton->SetKeyboardFocus();
        }

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            FInputModeUIOnly InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
        break;
    }

    // Log final state
    UE_LOG(LogTemp, Warning, TEXT("Current Switcher Index: %d"), MainSwitcher->GetActiveWidgetIndex());
    if (ResultsPanel)
        UE_LOG(LogTemp, Warning, TEXT("Results Panel Visibility: %d"), static_cast<int32>(ResultsPanel->GetVisibility()));
    if (StatsPanel)
        UE_LOG(LogTemp, Warning, TEXT("Stats Panel Visibility: %d"), static_cast<int32>(StatsPanel->GetVisibility()));
}





FText UEnd_Screen_Widget::FormatStatText(const FString& StatName, float CurrentValue, float NewValue)
{
    if (FMath::Abs(CurrentValue - NewValue) > SMALL_NUMBER)
    {
        return FText::FromString(FString::Printf(TEXT("%s %d > %d"),
            *StatName, FMath::RoundToInt(CurrentValue), FMath::RoundToInt(NewValue)));
    }
    return FText::FromString(FString::Printf(TEXT("%s %d"),
        *StatName, FMath::RoundToInt(CurrentValue)));
}
