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
    UE_LOG(LogTemp, Warning, TEXT("Final Score: %d, Current High Score: %d, Round: %d"),
        FinalScore, HighScore, RoundNumber);

    // Store the target values
    TargetScore = FinalScore;
    TargetHighScore = HighScore;
    TargetRound = RoundNumber;

    // Initialize current values and animation flags
    CurrentDisplayedRound = 0;
    CurrentDisplayedScore = 0;
    bIsAnimatingRound = false;
    bIsAnimatingScore = false;
    bIsAnimatingHighScore = false;

    // Set initial text values to 0
    if (RoundText)
        RoundText->SetText(FText::FromString("0"));
    if (FinalScoreText)
        FinalScoreText->SetText(FText::FromString("0"));
    if (HighScoreText)
        HighScoreText->SetText(FText::FromString("0"));

    // Start the game over sequence
    if (BlurAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting Blur Animation"));
        OnAnimationFinishedEvent.BindDynamic(this, &UEnd_Screen_Widget::OnBlurComplete);
        BindToAnimationFinished(BlurAnimation, OnAnimationFinishedEvent);
        PlayAnimation(BlurAnimation);
    }
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



void UEnd_Screen_Widget::SetWeaponLevel(int32 OldLevel, int32 NewLevel)
{
    if (WeaponLevelText)
    {
        FSlateColor GreenColor = FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // Pure green
        FSlateColor DefaultColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); // White

        // Ensure levels are valid (minimum level should be 1)
        OldLevel = FMath::Max(1, OldLevel);
        NewLevel = FMath::Max(1, NewLevel);

        // Only show level-up format if it's a genuine level increase and not initial display
        if (OldLevel != NewLevel && NewLevel > OldLevel && OldLevel > 0)
        {
            // Format with the arrow and set color to green for the new level
            FString ValueText = FString::Printf(TEXT("%d > %d"), OldLevel, NewLevel);
            WeaponLevelText->SetText(FText::FromString(ValueText));
            WeaponLevelText->SetColorAndOpacity(GreenColor);
        }
        else
        {
            // Just show the current level with default color
            WeaponLevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewLevel)));
            WeaponLevelText->SetColorAndOpacity(DefaultColor);
        }
    }
}




void UEnd_Screen_Widget::SetElementalLevels(int32 FireOldLevel, int32 FireNewLevel,
    int32 IceOldLevel, int32 IceNewLevel,
    int32 ThunderOldLevel, int32 ThunderNewLevel)
{

    UE_LOG(LogTemp, Warning, TEXT("Setting Elemental Levels:"));
    UE_LOG(LogTemp, Warning, TEXT("Fire: %d -> %d"), FireOldLevel, FireNewLevel);
    UE_LOG(LogTemp, Warning, TEXT("Ice: %d -> %d"), IceOldLevel, IceNewLevel);
    UE_LOG(LogTemp, Warning, TEXT("Thunder: %d -> %d"), ThunderOldLevel, ThunderNewLevel);

    FSlateColor GreenColor = FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // Pure green
    FSlateColor DefaultColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); // White

    // Fire Level
    if (FireProfText)
    {
        FireOldLevel = FMath::Max(1, FireOldLevel);
        FireNewLevel = FMath::Max(1, FireNewLevel);

        if (FireOldLevel != FireNewLevel && FireNewLevel > FireOldLevel)
        {
            FString ValueText = FString::Printf(TEXT("%d > %d"), FireOldLevel, FireNewLevel);
            FireProfText->SetText(FText::FromString(ValueText));
            FireProfText->SetColorAndOpacity(GreenColor);
            UE_LOG(LogTemp, Warning, TEXT("Fire Level increased: %s"), *ValueText);
        }
        else
        {
            FireProfText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FireNewLevel)));
            FireProfText->SetColorAndOpacity(DefaultColor);
        }
    }

    // Ice Level
    if (IceProfText)
    {
        IceOldLevel = FMath::Max(1, IceOldLevel);
        IceNewLevel = FMath::Max(1, IceNewLevel);

        if (IceOldLevel != IceNewLevel && IceNewLevel > IceOldLevel)
        {
            FString ValueText = FString::Printf(TEXT("%d > %d"), IceOldLevel, IceNewLevel);
            IceProfText->SetText(FText::FromString(ValueText));
            IceProfText->SetColorAndOpacity(GreenColor);
            UE_LOG(LogTemp, Warning, TEXT("Ice Level increased: %s"), *ValueText);
        }
        else
        {
            IceProfText->SetText(FText::FromString(FString::Printf(TEXT("%d"), IceNewLevel)));
            IceProfText->SetColorAndOpacity(DefaultColor);
        }
    }

    // Thunder Level
    if (ThunderProfText)
    {
        ThunderOldLevel = FMath::Max(1, ThunderOldLevel);
        ThunderNewLevel = FMath::Max(1, ThunderNewLevel);

        if (ThunderOldLevel != ThunderNewLevel && ThunderNewLevel > ThunderOldLevel)
        {
            FString ValueText = FString::Printf(TEXT("%d > %d"), ThunderOldLevel, ThunderNewLevel);
            ThunderProfText->SetText(FText::FromString(ValueText));
            ThunderProfText->SetColorAndOpacity(GreenColor);
            UE_LOG(LogTemp, Warning, TEXT("Thunder Level increased: %s"), *ValueText);
        }
        else
        {
            ThunderProfText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ThunderNewLevel)));
            ThunderProfText->SetColorAndOpacity(DefaultColor);
        }
    }
}








void UEnd_Screen_Widget::SetEXPEarned(float EXPAmount)
{

    if (EXPEarnedText)
    {
        EXPEarnedText->SetText(FText::FromString(FString::Printf(TEXT("EXP Earned: %.0f"), EXPAmount)));
    }

}



void UEnd_Screen_Widget::HandleHighScoreReveal(int32 StartValue, int32 EndValue)
{

    if (HighScoreText && !bIsAnimatingHighScore)
    {
        bIsAnimatingHighScore = true;
        CurrentDisplayedScore = StartValue;
        int32 ScoreIncrement = FMath::Max(1, (EndValue - StartValue) / 50);

        GetWorld()->GetTimerManager().SetTimer(
            HighScoreUpdateTimerHandle,
            [this, EndValue, ScoreIncrement]()
            {
                CurrentDisplayedScore = FMath::Min(CurrentDisplayedScore + ScoreIncrement, EndValue);
                HighScoreText->SetText(FText::AsNumber(CurrentDisplayedScore));

                if (TargetScore > TargetHighScore)
                {
                    HighScoreText->SetColorAndOpacity(FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)));
                }

                if (CurrentDisplayedScore >= EndValue)
                {
                    GetWorld()->GetTimerManager().ClearTimer(HighScoreUpdateTimerHandle);
                    bIsAnimatingHighScore = false;
                    OnHighScoreAnimationComplete();
                }
            },
            0.02f,
                true
             );
    }

}



void UEnd_Screen_Widget::SetCharacterImage(EWeaponType WeaponType)
{

    if (CharacterImage && CharacterImage2)
    {
        UTexture2D* CharacterTexture = (WeaponType == EWeaponType::Sword) ? SwordCharacterTexture : StaffCharacterTexture;

        if (CharacterTexture)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(CharacterTexture);
            CharacterImage->SetBrush(Brush);
            CharacterImage2->SetBrush(Brush);

            UE_LOG(LogTemp, Warning, TEXT("Setting character image for weapon type: %s"),
                *UEnum::GetValueAsString(WeaponType));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Character texture is null for weapon type: %s"),
                *UEnum::GetValueAsString(WeaponType));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterImage widget is null"));
    }

}




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

    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Main_Menu_Level"));

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
    StartResultsSequence();
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



void UEnd_Screen_Widget::UpdateStats(float Attack, float Defense, float Elemental, float Health, float NewAttack, float NewDefense, float NewElemental, float NewHealth)
{

    // Create a green color for increased stats
    FSlateColor GreenColor = FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // Pure green

    // Update attack text
    if (AttackText)
    {
        FString ValueText;
        if (FMath::Abs(Attack - NewAttack) > SMALL_NUMBER)
        {
            ValueText = FString::Printf(TEXT("%d > %d"),
                FMath::RoundToInt(Attack),
                FMath::RoundToInt(NewAttack));
            // Set the color of the entire text block to green if there's an increase
            if (NewAttack > Attack)
            {
                AttackText->SetColorAndOpacity(GreenColor);
            }
        }
        else
        {
            ValueText = FString::Printf(TEXT("%d"), FMath::RoundToInt(Attack));
        }
        AttackText->SetText(FText::FromString(ValueText));
    }

    // Update defense text
    if (DefenseText)
    {
        FString ValueText;
        if (FMath::Abs(Defense - NewDefense) > SMALL_NUMBER)
        {
            ValueText = FString::Printf(TEXT("%d > %d"),
                FMath::RoundToInt(Defense),
                FMath::RoundToInt(NewDefense));
            if (NewDefense > Defense)
            {
                DefenseText->SetColorAndOpacity(GreenColor);
            }
        }
        else
        {
            ValueText = FString::Printf(TEXT("%d"), FMath::RoundToInt(Defense));
        }
        DefenseText->SetText(FText::FromString(ValueText));
    }

    // Update elemental text
    if (ElementalText)
    {
        FString ValueText;
        if (FMath::Abs(Elemental - NewElemental) > SMALL_NUMBER)
        {
            ValueText = FString::Printf(TEXT("%d > %d"),
                FMath::RoundToInt(Elemental),
                FMath::RoundToInt(NewElemental));
            if (NewElemental > Elemental)
            {
                ElementalText->SetColorAndOpacity(GreenColor);
            }
        }
        else
        {
            ValueText = FString::Printf(TEXT("%d"), FMath::RoundToInt(Elemental));
        }
        ElementalText->SetText(FText::FromString(ValueText));
    }

    // Update health text
    if (HealthText)
    {
        FString ValueText;
        if (FMath::Abs(Health - NewHealth) > SMALL_NUMBER)
        {
            ValueText = FString::Printf(TEXT("%d > %d"),
                FMath::RoundToInt(Health),
                FMath::RoundToInt(NewHealth));
            if (NewHealth > Health)
            {
                HealthText->SetColorAndOpacity(GreenColor);
            }
        }
        else
        {
            ValueText = FString::Printf(TEXT("%d"), FMath::RoundToInt(Health));
        }

        HealthText->SetText(FText::FromString(ValueText));
    }
}



FText UEnd_Screen_Widget::FormatStatText(const FString& StatName, float CurrentValue, float NewValue)
{
    // If values are different, show the change
    if (FMath::Abs(CurrentValue - NewValue) > SMALL_NUMBER)
    {
        return FText::FromString(FString::Printf(TEXT("%d > %d"),
            FMath::RoundToInt(CurrentValue),
            FMath::RoundToInt(NewValue)));
    }

    // Otherwise just show the number
    return FText::FromString(FString::Printf(TEXT("%d"),
        FMath::RoundToInt(CurrentValue)));
}

void UEnd_Screen_Widget::StartResultsSequence()
{
    // Start with round animation
    AnimateRoundCount();

}

void UEnd_Screen_Widget::AnimateRoundCount()
{
    if (!bIsAnimatingRound && RoundText)
    {
        bIsAnimatingRound = true;
        GetWorld()->GetTimerManager().SetTimer(
            RoundUpdateTimerHandle,
            [this]()
            {
                CurrentDisplayedRound++;
                RoundText->SetText(FText::AsNumber(CurrentDisplayedRound));

                if (CurrentDisplayedRound >= TargetRound)
                {
                    GetWorld()->GetTimerManager().ClearTimer(RoundUpdateTimerHandle);
                    OnRoundAnimationComplete();
                }
            },
            0.05f,
                true
                );
    }

}

void UEnd_Screen_Widget::AnimateScoreCount()
{

    if (!bIsAnimatingScore && FinalScoreText)
    {
        bIsAnimatingScore = true;
        int32 ScoreIncrement = FMath::Max(1, TargetScore / 50);

        GetWorld()->GetTimerManager().SetTimer(
            ScoreUpdateTimerHandle,
            [this, ScoreIncrement]()
            {
                CurrentDisplayedScore = FMath::Min(CurrentDisplayedScore + ScoreIncrement, TargetScore);
                FinalScoreText->SetText(FText::AsNumber(CurrentDisplayedScore));

                if (CurrentDisplayedScore >= TargetScore)
                {
                    GetWorld()->GetTimerManager().ClearTimer(ScoreUpdateTimerHandle);
                    OnScoreAnimationComplete();
                }
            },
            0.02f,
                true
                );
    }


}

void UEnd_Screen_Widget::OnRoundAnimationComplete()
{

    bIsAnimatingRound = false;
    // Start score animation after a short delay
    FTimerHandle DelayTimer;
    GetWorld()->GetTimerManager().SetTimer(
        DelayTimer,
        [this]() { AnimateScoreCount(); },
        0.5f,
        false
    );


}

void UEnd_Screen_Widget::OnScoreAnimationComplete()
{

    bIsAnimatingScore = false;
    // Start high score animation after a delay
    FTimerHandle DelayTimer;
    GetWorld()->GetTimerManager().SetTimer(
        DelayTimer,
        [this]()
        {
            if (TargetScore > TargetHighScore)
            {
                HandleHighScoreReveal(TargetHighScore, TargetScore);
            }
            else
            {
                HandleHighScoreReveal(0, TargetHighScore);
            }
        },
        0.5f,
            false
            );


}


void UEnd_Screen_Widget::OnHighScoreAnimationComplete()
{

    // Wait and then show stats page
    GetWorld()->GetTimerManager().SetTimer(
        StatsPageTimerHandle,
        [this]() { ShowPage(EGameOverPage::Stats); },
        2.0f,
        false
    );

}
