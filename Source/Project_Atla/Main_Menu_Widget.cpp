// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/Gameplaystatics.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Game_Instance.h"
#include "Ren_Low_Poly_Character.h"
#include "Story_Board_Widget.h"
#include "Player_Save_Game.h"
#include "Carousel_Button_Widget.h"
#include "Components/AudioComponent.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/TextLocalizationManager.h"

#define LOCTEXT_NAMESPACE "MainMenu"


void UMain_Menu_Widget::NativeConstruct()
{

    Super::NativeConstruct();


    InitializeCanvasPanels();
    UpdateCanvasVisibility(0);

    bIsDemoBuild = true;

    // Load saved settings first
    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        GameInstance->LoadSettings();
        GameInstance->LoadPlayerProgress();
        GameInstance->LoadHighScores();

        // OVERRIDE VALUES FOR DEMO BUILD
        if (bIsDemoBuild)
        {
            // Override high scores to 0
            GameInstance->SwordHighScore = 0;
            //GameInstance->StaffHighScore = 0;

            // Override weapon proficiency for demo
            if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Sword))
            {
                FWeapon_Proficiency_Struct& SwordProficiency = GameInstance->WeaponProficiencyMap[EWeaponType::Sword];
                SwordProficiency.WeaponLevel = 3; // Fixed level 3
                SwordProficiency.CurrentEXP = 0;
                SwordProficiency.AttackPowerBoost = 6.0f; // Adjust as needed for level 3
                SwordProficiency.DefenseBoost = 3.0f;
                SwordProficiency.ElementalPowerBoost = 9.0f;
                SwordProficiency.MaxHealthBoost = 30.0f;
                SwordProficiency.MaxManaBoost = 45.0f;
            }

            // Override elemental proficiency for demo (all level 1)
            if (GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Sword))
            {
                FElemental_Proficiency_Struct& ElementalProf =
                    GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Sword];
                ElementalProf.FireLevel = 1;
                ElementalProf.IceLevel = 1;
                ElementalProf.ThunderLevel = 1;
                ElementalProf.FireProficiency = 1;
                ElementalProf.IceProficiency = 1;
                ElementalProf.ThunderProficiency = 1;
            }
        }
    }

    // Setup background music
    if (MenuMusic)
    {
        BackgroundMusic = UGameplayStatics::SpawnSound2D(GetWorld(), MenuMusic);
        if (BackgroundMusic)
        {
            float InitialVolume = 1.0f;
            if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
            {
                InitialVolume = GameInstance->GameSettings.MasterVolume;
            }
            BackgroundMusic->Play();
           // GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
               // FString::Printf(TEXT("Playing Music at Volume: %.2f"), InitialVolume));
        }
    }

    // Initialize settings controls
    InitializeSettingsControls();

    // Initialize menu buttons
    InitializeMenuButtons();

    //Check for demo build
    CheckAndApplyDemoRestrictions();

    // Set widget as focusable
    this->SetIsFocusable(true);
    bIsOnTitleScreen = true;
    bHasSetFocusForSwordButton = false;

    
    if (GetWorld())
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            // Set input mode to UI only with focus on this widget
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(this->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);

            // Make sure the widget has keyboard focus

            if (IsControllerConnected())
            {
                this->SetKeyboardFocus();
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Controller detected - focus set"));
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("No controller - mouse mode"));

            }
            // Debug message to confirm focus is set
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Widget focus set on startup"));
        }
    }
}



void UMain_Menu_Widget::InitializeMenuButtons()
{

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnPlayClicked);
    }

    if (SwordButton)
    {
        SwordButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnSwordButtonClicked);
        SwordButton->OnHovered.AddDynamic(this, &UMain_Menu_Widget::OnSwordButtonHovered);
    }

    if (StaffButton)
    {
        StaffButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnStaffButtonClicked);
        StaffButton->OnHovered.AddDynamic(this, &UMain_Menu_Widget::OnStaffButtonHovered);
    }

    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnSettingsClicked);
    }

    if (TutorialButton)

    {
        TutorialButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnTutorialClicked);
    }

    if (CreditsButton)

    {
        CreditsButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnCreditsClicked);
    }

    if (QuitButton)

    {
        QuitButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnQuitClicked);
       // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Quit Button Initialized"));
    }

    // Setup animations
    if (PressAnyButtonText)
    {
        PlayAnimation(PressAnyButtonFadeAnimation, 1.0f, 0);
    }

    if (TitleCanvas && TitleCanvasAnimation)
    {
        PlayAnimation(TitleCanvasAnimation);
    }


    if (ControlsButton)
    {
        ControlsButton->OnHovered.AddDynamic(this, &UMain_Menu_Widget::OnControlsButtonHovered);
        ControlsButton->OnUnhovered.AddDynamic(this, &UMain_Menu_Widget::OnControlsButtonUnhovered);

    }

    if (GameplayButton)
    {
        GameplayButton->OnHovered.AddDynamic(this, &UMain_Menu_Widget::OnGameplayButtonHovered);
    }

    if (LevellingUpButton)
    {
        LevellingUpButton->OnHovered.AddDynamic(this, &UMain_Menu_Widget::OnLevellingUpButtonHovered);
    }

    if (AttributesButton)
    {
        AttributesButton->OnHovered.AddDynamic(this, &UMain_Menu_Widget::OnAttributesButtonHovered);
    }

}



void UMain_Menu_Widget::InitializeSettingsControls()
{

    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        // Master Audio setup
        if (MasterAudioButton)
        {
            MasterAudioButton->OnHovered.AddDynamic(this, &UMain_Menu_Widget::OnMasterAudioButtonFocused);
        }

        if (MasterVolumeSlider)
        {
            MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnMasterVolumeChanged);
            MasterVolumeSlider->SetValue(GameInstance->GameSettings.MasterVolume);
            UpdateVolumeText(GameInstance->GameSettings.MasterVolume);
        }

        /*
        // Language Toggle setup
        if (LanguageToggle)
        {
            LanguageToggle->PossibleValues = { "English", "French", "German", "Spanish", "Japanese" };
            LanguageToggle->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnLanguageValueChanged);
            LanguageToggle->SetLabel(LOCTEXT("LanguageLabel", "Language").ToString());

            int32 LangIndex = LanguageToggle->PossibleValues.Find(GameInstance->GameSettings.CurrentLanguage);
            LanguageToggle->CurrentIndex = (LangIndex != INDEX_NONE) ? LangIndex : 0;
            LanguageToggle->UpdateDisplay();
        }
        */

        // Screen Shake Toggle setup
        if (ScreenShakeToggle)
        {
            ScreenShakeToggle->PossibleValues = { "ON", "OFF" };
            ScreenShakeToggle->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnScreenShakeValueChanged);
            ScreenShakeToggle->SetLabel(LOCTEXT("ScreenShakeLabel", "Screen Shake").ToString());
            ScreenShakeToggle->CurrentIndex = GameInstance->GameSettings.bScreenShakeEnabled ? 0 : 1;
            ScreenShakeToggle->UpdateDisplay();
        }

        // Vibration Toggle setup
        if (VibrationToggle)
        {
            VibrationToggle->PossibleValues = { "ON", "OFF" };
            VibrationToggle->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnVibrationValueChanged);
            VibrationToggle->SetLabel(LOCTEXT("VibrationLabel", "Vibration").ToString());
            VibrationToggle->CurrentIndex = GameInstance->GameSettings.bVibrationEnabled ? 0 : 1;
            VibrationToggle->UpdateDisplay();
        }

        // Settings buttons
        if (ResetToDefaultButton)
        {
            ResetToDefaultButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnResetToDefaultClicked);
        }

        if (ApplyChangesButton)
        {
            ApplyChangesButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnApplyChangesClicked);
        }
    }


}








void UMain_Menu_Widget::OnPlayClicked()
{
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(2);
        UpdateCanvasVisibility(2);

        // Only set focus if controller is connected
        if (IsControllerConnected())
        {
            if (SwordButton)
            {
                SwordButton->SetKeyboardFocus();
            }
        }
        LastFocusedButton = PlayButton;
    }

}



void UMain_Menu_Widget::OnTutorialClicked()
{

    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(5);
        UpdateCanvasVisibility(5);

        // Only set focus if controller is connected
        if (IsControllerConnected())
        {
            if (ControlsButton)
            {
                ControlsButton->SetKeyboardFocus();
            }
        }
        LastFocusedButton = TutorialButton;
    }

}





void UMain_Menu_Widget::OnSettingsClicked()
{

    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(4); // Settings is index 4
        UpdateCanvasVisibility(4);

        // Only set focus if controller is connected
        if (IsControllerConnected())
        {
            if (MasterAudioButton)
            {
                MasterAudioButton->SetKeyboardFocus();
            }
        }
        LastFocusedButton = SettingsButton;
    }

}



void UMain_Menu_Widget::OnCreditsClicked()
{


    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(3); // Settings is index 4
        UpdateCanvasVisibility(3);
        if (CreditsButton)
        {
            LastFocusedButton = CreditsButton;
        }
    }


}


void UMain_Menu_Widget::OnQuitClicked()
{
   
        UWorld* World = GetWorld();
        if (World)
        {
            APlayerController* PlayerController = World->GetFirstPlayerController();
            if (PlayerController)
            {
                // Quit the game
                PlayerController->ConsoleCommand("quit");
              //  GEngine->AddOnScreenDebugMessage(-1, 3.4f, FColor::Blue, TEXT("quit"));
            }
        }

       // GEngine->AddOnScreenDebugMessage(-1, 3.4f, FColor::Blue, TEXT("quit"));

}



void UMain_Menu_Widget::OnSwordButtonClicked()
{

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UGame_Instance* CustomGameInstance = Cast<UGame_Instance>(GameInstance);
        if (CustomGameInstance)
        {
            CustomGameInstance->SelectedWeapon = EWeaponType::Sword; // Set weapon to Sword
        }
    }

    // Open the gameplay level
   // UGameplayStatics::OpenLevel(this, FName("LowPoly_Level"));

    ShowStoryBoardWidget();


}



void UMain_Menu_Widget::OnSwordButtonHovered()
{
    UpdateWeaponStats(EWeaponType::Sword);
    OnWeaponButtonHovered(LOCTEXT("SwordDesc", "A sharp sword with enhanced lightning power.").ToString());

    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null in OnSwordButtonHovered!"));
        return;
    }

    // Check if demo build and override values
    if (bIsDemoBuild)
    {
        // Set demo weapon level
        if (WeaponLevelStat)
        {
            WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), 3))); // Fixed level 3 for demo
        }

        // Set demo high score
        if (HighScore)
        {
            HighScore->SetText(FText::FromString(TEXT("0")));
        }

        // Set demo elemental levels (all at level 1 for demo)
        if (FireProficiencyLevel)
            FireProficiencyLevel->SetText(FText::FromString(TEXT("1")));
        if (IceProficiencyLevel)
            IceProficiencyLevel->SetText(FText::FromString(TEXT("1")));
        if (ThunderProficiencyLevel)
            ThunderProficiencyLevel->SetText(FText::FromString(TEXT("1")));
    }
    else
    {
        // Normal game - use saved data
        if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Sword))
        {
            const FWeapon_Proficiency_Struct& SwordStats = GameInstance->WeaponProficiencyMap[EWeaponType::Sword];
            if (WeaponLevelStat)
            {
                WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordStats.WeaponLevel)));
            }
        }

        // Update high score for normal game
        if (HighScore)
        {
            int32 SwordHighScoreValue = GameInstance->SwordHighScore;
            HighScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordHighScoreValue)));
        }

        // Update elemental levels for normal game
        if (GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Sword))
        {
            const FElemental_Proficiency_Struct& ElementalStats =
                GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Sword];

            if (FireProficiencyLevel)
                FireProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.FireLevel)));
            if (IceProficiencyLevel)
                IceProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.IceLevel)));
            if (ThunderProficiencyLevel)
                ThunderProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.ThunderLevel)));
        }
    }

    // Update character render
    if (CharacterWeaponRender && SwordCharacterTexture)
    {
        CharacterWeaponRender->SetBrushFromTexture(SwordCharacterTexture);
    }
}



void UMain_Menu_Widget::OnSwordButtonFocused()
{
    UpdateWeaponStats(EWeaponType::Sword);
    OnWeaponButtonHovered(LOCTEXT("SwordDesc", "A sharp sword with enhanced lightning power.").ToString());

    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null in OnSwordButtonHovered!"));
        return;
    }

    // Check if demo build and override values
    if (bIsDemoBuild)
    {
        // Set demo weapon level
        if (WeaponLevelStat)
        {
            WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), 3))); // Fixed level 3 for demo
        }

        // Set demo high score
        if (HighScore)
        {
            HighScore->SetText(FText::FromString(TEXT("-")));
        }

        // Set demo elemental levels (all at level 1 for demo)
        if (FireProficiencyLevel)
            FireProficiencyLevel->SetText(FText::FromString(TEXT("1")));
        if (IceProficiencyLevel)
            IceProficiencyLevel->SetText(FText::FromString(TEXT("1")));
        if (ThunderProficiencyLevel)
            ThunderProficiencyLevel->SetText(FText::FromString(TEXT("1")));
    }
    else
    {
        // Normal game - use saved data
        if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Sword))
        {
            const FWeapon_Proficiency_Struct& SwordStats = GameInstance->WeaponProficiencyMap[EWeaponType::Sword];
            if (WeaponLevelStat)
            {
                WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordStats.WeaponLevel)));
            }
        }

        // Update high score for normal game
        if (HighScore)
        {
            int32 SwordHighScoreValue = GameInstance->SwordHighScore;
            HighScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordHighScoreValue)));
        }

        // Update elemental levels for normal game
        if (GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Sword))
        {
            const FElemental_Proficiency_Struct& ElementalStats =
                GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Sword];

            if (FireProficiencyLevel)
                FireProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.FireLevel)));
            if (IceProficiencyLevel)
                IceProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.IceLevel)));
            if (ThunderProficiencyLevel)
                ThunderProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.ThunderLevel)));
        }
    }

    // Update character render
    if (CharacterWeaponRender && SwordCharacterTexture)
    {
        CharacterWeaponRender->SetBrushFromTexture(SwordCharacterTexture);
    }
}

void UMain_Menu_Widget::OnStaffButtonClicked()
{


    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UGame_Instance* CustomGameInstance = Cast<UGame_Instance>(GameInstance);
        if (CustomGameInstance)
        {
            CustomGameInstance->SelectedWeapon = EWeaponType::Staff; // Set weapon to Sword
        }
    }

    // Open the gameplay level
   // UGameplayStatics::OpenLevel(this, FName("LowPoly_Level"));

    ShowStoryBoardWidget();


}



void UMain_Menu_Widget::OnStaffButtonHovered()
{

    UpdateWeaponStats(EWeaponType::Staff);
    OnWeaponButtonHovered(LOCTEXT("StaffDesc", "A mystical staff that boosts elemental power.").ToString());
   // GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Staff Button Hovered"));

    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());

    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null in OnStaffButtonHovered!"));
        return;
    }

    // Check if the weapon data exists in the proficiency map
    if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Staff))
    {
        const FWeapon_Proficiency_Struct& SwordStats = GameInstance->WeaponProficiencyMap[EWeaponType::Staff];

        // Update the weapon level text
        if (WeaponLevelStat)
        {
            WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordStats.WeaponLevel)));
            UE_LOG(LogTemp, Log, TEXT("Updated WeaponLevelStat with Staff Level: %d"), SwordStats.WeaponLevel);
        }
        // Update elemental levels
        if (GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Staff))
        {
            const FElemental_Proficiency_Struct& ElementalStats =
                GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Staff];

            if (FireProficiencyLevel)
                FireProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.FireLevel)));

            if (IceProficiencyLevel)
                IceProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.IceLevel)));

            if (ThunderProficiencyLevel)
                ThunderProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.ThunderLevel)));

            UE_LOG(LogTemp, Log, TEXT("Updated Staff Elemental Levels - Fire: %d, Ice: %d, Thunder: %d"),
                ElementalStats.FireLevel, ElementalStats.IceLevel, ElementalStats.ThunderLevel);
        }
    }


    // Update high score for staff
    if (HighScore)
    {
        // Get the high score from Game Instance
        int32 StaffHighScoreValue = GameInstance->StaffHighScore;
        HighScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), StaffHighScoreValue)));
        UE_LOG(LogTemp, Log, TEXT("Updated HighScore with Staff High Score: %d"), StaffHighScoreValue);
    }


    if (CharacterWeaponRender && StaffCharacterTexture)

    {

        CharacterWeaponRender->SetBrushFromTexture(StaffCharacterTexture);

    }

}



void UMain_Menu_Widget::OnStaffButtonFocused()
{


    UpdateWeaponStats(EWeaponType::Staff);
    OnWeaponButtonHovered(LOCTEXT("StaffDesc", "A mystical staff that boosts elemental power.").ToString());
  //  GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Staff Button Focused"));


    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());

    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null in OnSwordButtonHovered!"));
        return;
    }

    // Check if the weapon data exists in the proficiency map
    if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Staff))
    {
        const FWeapon_Proficiency_Struct& SwordStats = GameInstance->WeaponProficiencyMap[EWeaponType::Staff];

        // Update the weapon level text
        if (WeaponLevelStat)
        {
            WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordStats.WeaponLevel)));
            UE_LOG(LogTemp, Log, TEXT("Updated WeaponLevelStat with Sword Level: %d"), SwordStats.WeaponLevel);
        }
        // Update elemental levels
        if (GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Staff))
        {
            const FElemental_Proficiency_Struct& ElementalStats =
                GameInstance->WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Staff];

            if (FireProficiencyLevel)
                FireProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.FireLevel)));

            if (IceProficiencyLevel)
                IceProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.IceLevel)));

            if (ThunderProficiencyLevel)
                ThunderProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ElementalStats.ThunderLevel)));

            UE_LOG(LogTemp, Log, TEXT("Updated Staff Elemental Levels - Fire: %d, Ice: %d, Thunder: %d"),
                ElementalStats.FireLevel, ElementalStats.IceLevel, ElementalStats.ThunderLevel);
        }
    }


    // Update high score for staff
    if (HighScore)
    {
        // Get the high score from Game Instance
        int32 StaffHighScoreValue = GameInstance->StaffHighScore;
        HighScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), StaffHighScoreValue)));
        UE_LOG(LogTemp, Log, TEXT("Updated HighScore with Staff High Score: %d"), StaffHighScoreValue);
    }


    if (CharacterWeaponRender && StaffCharacterTexture)

    {

        CharacterWeaponRender->SetBrushFromTexture(StaffCharacterTexture);

    }
}





void UMain_Menu_Widget::PlayNavigationSound()
{


    if (NavigationSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), NavigationSound);
    }


}




void UMain_Menu_Widget::PlayBackSound()
{

    if (BackSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), BackSound);
    }


}





void UMain_Menu_Widget::OnWeaponButtonHovered(const FString& Description)
{

    if (WeaponDescription)
    {
        WeaponDescription->SetText(FText::FromString(Description));
    }

}



void UMain_Menu_Widget::OnControlsButtonHovered()
{

   // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Controls Button Hovered"));

    // Update title and description
    UpdateTutorialContent(
        LOCTEXT("TutorialControls", "Controls"),
        FText::GetEmpty()
    );


    // Show the controls container
    if (ControlsContainer)
    {
        ControlsContainer->SetVisibility(ESlateVisibility::Visible);
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Controls Container Shown"));
    }
    else
    {
       // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Controls Container is null!"));
    }
}




void UMain_Menu_Widget::OnControlsButtonUnhovered()
{

    // Hide the container when no longer hovering
    if (ControlsContainer)
    {
        ControlsContainer->SetVisibility(ESlateVisibility::Hidden);
       // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Controls Container Hidden"));
    }
    else
    {
       // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Controls Container is null!"));
    }

    UpdateTutorialContent(
        FText::GetEmpty(),
        FText::GetEmpty()
    );
}



void UMain_Menu_Widget::OnGameplayButtonHovered()
{

    // Hide controls container if visible
    if (ControlsContainer)
    {
        ControlsContainer->SetVisibility(ESlateVisibility::Hidden);
    }

    UpdateTutorialContent(
        LOCTEXT("TutorialGameplay", "Gameplay"),
        LOCTEXT("TutorialGameplayDesc", "Objective:\n-Your goal is to survive wave after wave of enemies. Stay alert, defeat as many foes as possible, and aim for a new high score.\n\n"
            "Combat Basics:\n"
            "-Use basic attacks to quickly take down enemies\n"
            "-Exploit enemy weaknesses with elemental attacks (Fire, Ice, or Thunder)\n"
            "-As you use these attacks, your Elemental attribute increases, unlocking more powerful moves\n\n"
            "Using Techniques:\n-When your weapon levels up, new techniques become available. These special moves help you inflict heavy damage and keep enemies at bay.\n\n"
            "Abilities:\n-Use your abilities strategically during battle. When timed correctly, they can turn the tide by maximizing damage and controlling enemy movement.\n\n"
            "End of Battle:\n-The game continues until your Health runs out. At that point, you'll see an end screen displaying your final score, your best high score, and a summary of your attributes, including your Weapon Level and Elemental proficiency.")
    );

}



void UMain_Menu_Widget::OnLevellingUpButtonHovered()
{

    // Hide controls container if visible
    if (ControlsContainer)
    {
        ControlsContainer->SetVisibility(ESlateVisibility::Hidden);
    }

    UpdateTutorialContent(
        LOCTEXT("TutorialLevelUp", "Levelling Up"),
        LOCTEXT("TutorialLevelUpDesc", "Earn Experience:\n-Defeat enemies to gain experience points for your chosen weapon.\n\n"
            "Increase Your Weapon Level:\n-When you collect enough experience, your weapon will level up. Leveling up unlocks new combat techniques that help you deal more damage.\n\n"
            "Boost Your Elemental Proficiency:\n-Use elemental attacks (Fire, Ice, or Thunder) during battle. Each use increases your Elemental attribute for that weapon.\n\n"
            "Unlock Powerful Attacks:\n-As your elemental proficiency grows, you'll gain access to stronger elemental moves. Keep using a specific element to make its attacks even more effective.")
    );



}



void UMain_Menu_Widget::OnAttributesButtonHovered()
{
    // Hide controls container if visible
    if (ControlsContainer)
    {
        ControlsContainer->SetVisibility(ESlateVisibility::Hidden);
    }

    UpdateTutorialContent(
        LOCTEXT("TutorialAttributes", "Attributes"),
        LOCTEXT("TutorialAttributesDesc", "Health:\n-Your Health (HP) shows how much damage you can take. Taking damage in battle reduces your Health. When your Health reaches zero, it's game over.\n\n"
            "Attack:\n-The Attack attribute determines how much damage you deal to enemies. Increasing this attribute means you can defeat foes more quickly.\n\n"
            "Defence:\n-Your Defence attribute reduces the damage you take from enemy attacks. A higher Defence helps you withstand more hits during combat.\n\n"
            "Elemental:\n-This attribute measures your proficiency with elemental attacks (Fire, Ice, and Thunder). As you use these attacks, your Elemental attribute improves, unlocking even more potent moves.\n\n"
            "Weapon Level:\n-Your Weapon Level reflects the experience you've earned with your chosen weapon. As the level increases, new techniques and abilities become available, enhancing your combat options.")
    );

}



void UMain_Menu_Widget::OnControlsButtonFocused()
{

    OnControlsButtonHovered();

}




void UMain_Menu_Widget::OnGameplayButtonFocused()
{


    OnGameplayButtonHovered();



}



void UMain_Menu_Widget::OnLevellingUpButtonFocused()
{


    OnLevellingUpButtonHovered();



}




void UMain_Menu_Widget::OnAttributesButtonFocused()
{

    OnAttributesButtonHovered();


}



void UMain_Menu_Widget::UpdateTutorialContent(const FText& Title, const FText& Description)
{

    if (TutorialTitleText)
    {
        TutorialTitleText->SetText(Title);
    }
    if (TutorialDescriptionText)
    {
        TutorialDescriptionText->SetText(Description);
    }

  
}



void UMain_Menu_Widget::ScrollTutorialContent(float Value)
{

    if (TutorialScrollBox)
    {
        // Adjust this multiplier to control scroll speed
        const float ScrollSpeed = 20.0f;
        float CurrentOffset = TutorialScrollBox->GetScrollOffset();
        float NewOffset = CurrentOffset + (Value * ScrollSpeed);

        // Clamp the offset between 0 and the maximum scroll
        float MaxScroll = TutorialScrollBox->GetScrollOffsetOfEnd();
        NewOffset = FMath::Clamp(NewOffset, 0.0f, MaxScroll);

        TutorialScrollBox->SetScrollOffset(NewOffset);
    }


}




void UMain_Menu_Widget::ShowStoryBoardWidget()
{

    // Check if we have a valid story board widget class
    if (!StoryBoardWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("StoryBoardWidgetClass is not set! Please set it in the Blueprint."));
        // Fallback to directly opening the level if widget class is missing
        UGameplayStatics::OpenLevel(this, FName("LowPoly_Level"));
        return;
    }

    // Create the story board widget
    if (UStory_Board_Widget* StoryWidget = CreateWidget<UStory_Board_Widget>(GetWorld(), StoryBoardWidgetClass))
    {
        // Configure the widget
        StoryWidget->LevelToOpen = FName("LowPoly_Level");
        StoryWidget->StoryTextContent = FText::FromString(TEXT(
            "Consciousness returns slowly. You find yourself on an unfamiliar shore, surrounded by an island that pulses with danger.\n\n"
            "Creatures stalk through the mist. Your past is a void - yet your hands remember the weight of weapons, your body knows abilities that feel both foreign and familiar.\n\n"
            "Every breath is survival. Every step forward brings new questions. This island holds answers... if you can live long enough to find them."
        ));

        // Hide the main menu and show the story widget
        this->RemoveFromParent();
        StoryWidget->AddToViewport(10); // Higher Z-order to ensure it's on top

        // Start the story sequence
        StoryWidget->StartStorySequence();

        // Stop the main menu music if it's playing
        if (BackgroundMusic)
        {
            BackgroundMusic->Stop();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Story Board Widget!"));
        // Fallback to directly opening the level
        UGameplayStatics::OpenLevel(this, FName("LowPoly_Level"));
    }



}



bool UMain_Menu_Widget::IsControllerConnected() const
{
    return FSlateApplication::Get().IsGamepadAttached();
}



void UMain_Menu_Widget::UpdateWeaponStats(EWeaponType WeaponType)
{
    // Get the Game Instance
    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());

    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance in UpdateWeaponStats!"));
        return;
    }


    // Retrieve updated stats
    float UpdatedAttack = 0.0f;
    float UpdatedDefense = 0.0f;
    float UpdatedElementalAttack = 0.0f;
    float UpdatedMaxHealth = 0.0f;  
    int32 UpdatedWeaponLevel = 0;


    if (GameInstance->GetWeaponBaseStats(WeaponType, UpdatedAttack, UpdatedDefense, UpdatedElementalAttack, UpdatedWeaponLevel))
    {
        // Update the UI with the retrieved stats
        UpdateWeaponStatsText(UpdatedAttack, UpdatedDefense, UpdatedElementalAttack, UpdatedWeaponLevel);
        UpdateElementalProficiencyText(WeaponType);  // Add this line
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve updated stats for WeaponType: %d"), (int32)WeaponType);
    }


    // Update high score based on weapon type
    if (HighScore && GameInstance)
    {
        int32 HighScoreValue = 0;

        if (WeaponType == EWeaponType::Sword)
        {
            HighScoreValue = GameInstance->SwordHighScore;
        }
        else if (WeaponType == EWeaponType::Staff)
        {
            HighScoreValue = GameInstance->StaffHighScore;
        }

        HighScore->SetText(FText::FromString(FString::Printf(TEXT("%d"), HighScoreValue)));
        UE_LOG(LogTemp, Log, TEXT("Updated HighScore for %s with value: %d"),
            *UEnum::GetValueAsString(WeaponType), HighScoreValue);
    }
}




void UMain_Menu_Widget::InitializeCanvasPanels()
{


    // Initialize the array of canvas panels in the order they appear in the widget switcher
    MenuCanvases = {
         TitleCanvas,
    MainMenuCanvas,
    WeaponSelectCanvas,
    CreditsCanvas,
    SettingsCanvas,
    TutorialCanvas
    };

    // Initially hide all canvas panels
    for (UCanvasPanel* Canvas : MenuCanvases)
    {
        if (Canvas)
        {
            Canvas->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Show the title screen canvas initially
    if (TitleCanvas)
    {
        TitleCanvas->SetVisibility(ESlateVisibility::Visible);
        if (TitleCanvasAnimation)
        {
            PlayAnimation(TitleCanvasAnimation);
        }
        if (PressAnyButtonFadeAnimation)
        {
            PlayAnimation(PressAnyButtonFadeAnimation);
        }

    }
}





void UMain_Menu_Widget::UpdateVolumeText(float Volume)
{

    if (VolumePercentageText)
    {
        int32 VolumePercent = FMath::RoundToInt(Volume * 100.0f);
        VolumePercentageText->SetText(FText::Format(LOCTEXT("VolumePercentFormat", "{0}%"), FText::AsNumber(VolumePercent)));
    }

}



void UMain_Menu_Widget::OnMasterVolumeChanged(float Value)
{
    UpdateVolumeText(Value);

    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        GameInstance->GameSettings.MasterVolume = Value;

        // Update both the global sound and the background music
        UGameplayStatics::SetSoundMixClassOverride(GetWorld(), nullptr, nullptr, Value, 1.0f, 0.0f);

        // Also update background music directly
        if (BackgroundMusic)
        {
            // If volume was 0 and is now being increased, make sure audio is playing
            if (Value > 0.0f && !BackgroundMusic->IsPlaying())
            {
                BackgroundMusic->Play();
            }
            BackgroundMusic->SetVolumeMultiplier(Value);
        }

       // GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
          //  FString::Printf(TEXT("Volume Changed: %.2f"), Value));
    }
}




void UMain_Menu_Widget::UpdateCanvasVisibility(int32 ActiveIndex)
{


    // Validate index
    if (ActiveIndex < 0 || ActiveIndex >= MenuCanvases.Num())
    {
        return;
    }

    // Hide all canvases first
    for (UCanvasPanel* Canvas : MenuCanvases)
    {
        if (Canvas)
        {
            Canvas->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Show the active canvas and play its animation
    if (UCanvasPanel* ActiveCanvas = MenuCanvases[ActiveIndex])
    {
        ActiveCanvas->SetVisibility(ESlateVisibility::Visible);

        // Play the appropriate animation based on the active index
        switch (ActiveIndex)
        {
        case 0: // Title
            if (TitleCanvasAnimation)
                PlayAnimation(TitleCanvasAnimation);
            break;
        case 1: // Main Menu
            if (MainMenuCanvasAnimation)
                PlayAnimation(MainMenuCanvasAnimation);
            break;
        case 2: // Weapon Select
            if (WeaponSelectCanvasAnimation)
                PlayAnimation(WeaponSelectCanvasAnimation);
            break;
        case 3: // Credits
            if (CreditsCanvasAnimation)
                PlayAnimation(CreditsCanvasAnimation);
            break;
        case 4: // Settings
            if (SettingsCanvasAnimation)
                PlayAnimation(SettingsCanvasAnimation);
            break;
        case 5: // Tutorial
            if (TutorialCanvasAnimation)
                PlayAnimation(TutorialCanvasAnimation);
            break;
        }
    }


}




void UMain_Menu_Widget::UpdateWeaponStatsText(float Attack, float Defense, float ElementalAttack, int32 WeaponLevel)
{

    if (AttackStat)
    {
        // Convert Attack to an integer and update the text
        AttackStat->SetText(FText::Format(LOCTEXT("AttackStatFormat", "{0}"), FText::AsNumber(static_cast<int32>(Attack))));
        UE_LOG(LogTemp, Warning, TEXT("AttackStat updated with: %d"), static_cast<int32>(Attack));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AttackStat is null!"));
    }

    if (DefenceStat)
    {
        // Convert Defense to an integer and update the text
        DefenceStat->SetText(FText::Format(LOCTEXT("DefenseStatFormat", "{0}"), FText::AsNumber(static_cast<int32>(Defense))));
        UE_LOG(LogTemp, Warning, TEXT("DefenceStat updated with: %d"), static_cast<int32>(Defense));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DefenceStat is null!"));
    }

    if (ElementalStat)
    {
        // Convert ElementalAttack to an integer and update the text
        ElementalStat->SetText(FText::Format(LOCTEXT("ElementalStatFormat", "{0}"), FText::AsNumber(static_cast<int32>(ElementalAttack))));
        UE_LOG(LogTemp, Warning, TEXT("ElementalStat updated with: %d"), static_cast<int32>(ElementalAttack));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ElementalStat is null!"));
    }

    if (WeaponLevelStat)
    {
        // WeaponLevel is already an integer, so no conversion needed
        WeaponLevelStat->SetText(FText::Format(LOCTEXT("WeaponLevelFormat", "{0}"), FText::AsNumber(WeaponLevel)));
        UE_LOG(LogTemp, Warning, TEXT("WeaponLevelStat updated with: %d"), WeaponLevel);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponLevelStat is null!"));
    }
}




void UMain_Menu_Widget::UpdateElementalProficiencyText(EWeaponType WeaponType)
{

    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null in UpdateElementalProficiencyText!"));
        return;
    }

    int32 FireLevel = 1;
    int32 IceLevel = 1;
    int32 ThunderLevel = 1;

    if (GameInstance->GetElementalProficiencyLevels(WeaponType, FireLevel, IceLevel, ThunderLevel))
    {
        if (FireProficiencyLevel)
        {
            FireProficiencyLevel->SetText(FText::Format(LOCTEXT("FireLevelFormat", "{0}"), FText::AsNumber(FireLevel)));
            UE_LOG(LogTemp, Warning, TEXT("FireProficiencyLevel updated with: %d"), FireLevel);
        }

        if (IceProficiencyLevel)
        {
            IceProficiencyLevel->SetText(FText::Format(LOCTEXT("IceLevelFormat", "{0}"), FText::AsNumber(IceLevel)));
            UE_LOG(LogTemp, Warning, TEXT("IceProficiencyLevel updated with: %d"), IceLevel);
        }

        if (ThunderProficiencyLevel)
        {
            ThunderProficiencyLevel->SetText(FText::Format(LOCTEXT("ThunderLevelFormat", "{0}"), FText::AsNumber(ThunderLevel)));
            UE_LOG(LogTemp, Warning, TEXT("ThunderProficiencyLevel updated with: %d"), ThunderLevel);
        }
    }



}



void UMain_Menu_Widget::SaveAudioSettings()
{

   

}




void UMain_Menu_Widget::LoadAudioSettings()
{

   

}


void UMain_Menu_Widget::UpdateCharacterImage()
{

    if (!CharacterWeaponRender) return;

    if (SwordButton && SwordButton->HasKeyboardFocus() && SwordCharacterTexture)
    {
        CharacterWeaponRender->SetBrushFromTexture(SwordCharacterTexture);
    }
    else if (StaffButton && StaffButton->HasKeyboardFocus() && StaffCharacterTexture)
    {
        CharacterWeaponRender->SetBrushFromTexture(StaffCharacterTexture);
    }

}







void UMain_Menu_Widget::HandleGoBack()
{
    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        if (CurrentIndex > 1)
        {
            WidgetSwitcher->SetActiveWidgetIndex(1);  // Go back to main menu
            UpdateCanvasVisibility(1);

            // Only restore focus if controller is connected
            if (IsControllerConnected())
            {
                if (LastFocusedButton)
                {
                    LastFocusedButton->SetKeyboardFocus();
                    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
                    //    FString::Printf(TEXT("Focus restored to: %s"), *LastFocusedButton->GetName()));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot go back further from the main menu!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WidgetSwitcher is null!"));
    }

}






void UMain_Menu_Widget::SwitchToMainMenu()
{

    if (WidgetSwitcher && bIsOnTitleScreen)
    {
        WidgetSwitcher->SetActiveWidgetIndex(1); // Switch to Main Menu (index 1)
        UpdateCanvasVisibility(1);
        LastFocusedButton = nullptr;

        // Check for controller
        if (IsControllerConnected())
        {
            if (PlayButton)
            {
                PlayButton->SetKeyboardFocus();
            }
        }
        else
        {
            FSlateApplication::Get().ClearKeyboardFocus();

            // Optional debug message
            //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("No controller - cleared focus"));
        }

        bIsOnTitleScreen = false;
    }


}





void UMain_Menu_Widget::SwitchToWeaponSelectMenu()
{

    // Check if we're on Widget 2 (index 2)
    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        if (CurrentIndex == 2 && !bHasSetFocusForSwordButton)
        {
            // Only set focus if controller is connected
            if (IsControllerConnected())
            {
                if (SwordButton)
                {
                    SwordButton->SetKeyboardFocus();
                    bHasSetFocusForSwordButton = true;  // Mark that focus has been set
                    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Sword button focused!"));
                }
            }
        }
        else if (CurrentIndex != 2)
        {
            // Reset the flag if we're not on widget 2
            bHasSetFocusForSwordButton = false;
        }
    }


}





void UMain_Menu_Widget::OnScreenShakeValueChanged(const FString& NewValue)
{

    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        // Update the game setting
        GameInstance->GameSettings.bScreenShakeEnabled = (NewValue == "ON");

        // Save settings
      //  GameInstance->SaveSettings();

        // Debug message
       // GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
      //      FString::Printf(TEXT("Screen Shake: %s"), *NewValue));
    }


}


void UMain_Menu_Widget::OnVibrationValueChanged(const FString& NewValue)
{

    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        // Update the game setting
        GameInstance->GameSettings.bVibrationEnabled = (NewValue == "ON");

        // Save settings
       // GameInstance->SaveSettings();

        // Debug message
       // GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
          //  FString::Printf(TEXT("Vibration: %s"), *NewValue));
    }



}


void UMain_Menu_Widget::OnLanguageValueChanged(const FString& NewValue)
{
    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        // Update the game setting
        GameInstance->GameSettings.CurrentLanguage = NewValue;

        // Map language name to culture code
        FString CultureCode;
        if (NewValue == "English") CultureCode = "en";
        else if (NewValue == "French") CultureCode = "fr";
        else if (NewValue == "German") CultureCode = "de";
        else if (NewValue == "Spanish") CultureCode = "es";
        else if (NewValue == "Japanese") CultureCode = "ja";

        // Apply the culture change
        const bool bIsSuccessful = FInternationalization::Get().SetCurrentLanguage(CultureCode);

        // Force UI refresh
        if (WidgetSwitcher)
        {
            int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();
            UpdateCanvasVisibility(CurrentIndex);
        }

        // Save settings
        GameInstance->SaveSettings();
    }



}



void UMain_Menu_Widget::OnResetToDefaultClicked()
{

    ResetSettingsToDefault();

    // Save settings
    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        GameInstance->SaveSettings();
    }

   // GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Settings Reset to Default"));


}




void UMain_Menu_Widget::OnApplyChangesClicked()
{

    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        GameInstance->SaveSettings();
    }

  //  GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Settings Applied"));

}




void UMain_Menu_Widget::AdjustMasterVolume(bool bIncrease)
{

    if (MasterVolumeSlider)
    {
        float CurrentValue = MasterVolumeSlider->GetValue();
        float Step = 0.05f;

        if (bIncrease)
        {
            CurrentValue = FMath::Min(CurrentValue + Step, 1.0f);
        }
        else
        {
            CurrentValue = FMath::Max(CurrentValue - Step, 0.0f);
        }

        MasterVolumeSlider->SetValue(CurrentValue);
        UpdateVolumeText(CurrentValue);

        // Apply volume changes
        if (BackgroundMusic)
        {
            if (CurrentValue > 0.0f && !BackgroundMusic->IsPlaying())
            {
                BackgroundMusic->Play();
            }
            BackgroundMusic->SetVolumeMultiplier(CurrentValue);
        }

        UGameplayStatics::SetSoundMixClassOverride(GetWorld(), nullptr, nullptr, CurrentValue, 1.0f, 0.0f);
    }

}




void UMain_Menu_Widget::OnMasterAudioButtonFocused()
{


    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        // Optional: Update the UI to show current volume
        if (MasterVolumeSlider)
        {
            UpdateVolumeText(MasterVolumeSlider->GetValue());
        }

        // Debug message to confirm button is focused
      //  GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Master Audio Button Focused"));
    }


}





void UMain_Menu_Widget::ResetSettingsToDefault()
{

    // Reset Master Volume
    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->SetValue(1.0f);  // 100%
        UpdateVolumeText(1.0f);
    }

    // Reset Screen Shake
    if (ScreenShakeToggle)
    {
        ScreenShakeToggle->CurrentIndex = 0;  // ON
        ScreenShakeToggle->UpdateDisplay();
        if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
        {
            GameInstance->GameSettings.bScreenShakeEnabled = true;
        }
    }

    // Reset Vibration
    if (VibrationToggle)
    {
        VibrationToggle->CurrentIndex = 0;  // ON
        VibrationToggle->UpdateDisplay();
        if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
        {
            GameInstance->GameSettings.bVibrationEnabled = true;
        }
    }

    /*
    // Reset Language
    if (LanguageToggle)
    {
        LanguageToggle->CurrentIndex = 0;  // English
        LanguageToggle->UpdateDisplay();
        if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
        {
            GameInstance->GameSettings.CurrentLanguage = "English";
        }
    }



}*/

}




FReply UMain_Menu_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

    // Get the pressed key
    FKey PressedKey = InKeyEvent.GetKey();


    if (PressedKey == EKeys::Gamepad_FaceButton_Top)
    {
        // Either ignore this button completely
        return FReply::Handled();
    }
   

    // First check if we're on the title screen
    if (bIsOnTitleScreen)
    {
        // On title screen, any key will move to main menu (except ESC which is typically handled by the engine)
        if (PressedKey != EKeys::Escape)
        {
            SwitchToMainMenu();
            PlayNavigationSound(); // Optional - play a sound when transitioning
           // GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
              //  FString::Printf(TEXT("Key pressed: %s"), *PressedKey.ToString()));
            return FReply::Handled();
        }
    }
    else
    {
        // Not on title screen - handle regular navigation
        if (PressedKey == EKeys::Gamepad_FaceButton_Right)
        {
            HandleGoBack();
           // GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Back!"));
            PlayBackSound();
            return FReply::Handled();
        }

        // Master Audio Controls
        if (MasterAudioButton && MasterAudioButton->HasKeyboardFocus())
        {
            if (PressedKey == EKeys::Gamepad_DPad_Left ||
                PressedKey == EKeys::Gamepad_LeftStick_Left ||
                PressedKey == EKeys::A)
            {
                AdjustMasterVolume(false);
                return FReply::Handled();
            }
            else if (PressedKey == EKeys::Gamepad_DPad_Right ||
                PressedKey == EKeys::Gamepad_LeftStick_Right ||
                PressedKey == EKeys::D)
            {
                AdjustMasterVolume(true);
                return FReply::Handled();
            }
        }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

}

FReply UMain_Menu_Widget::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogInputEvent)
{
    if (TutorialScrollBox)
    {
        // Check if it's the right stick's Y-axis
        if (InAnalogInputEvent.GetUserIndex() == 0 &&
            (InAnalogInputEvent.GetKey() == EKeys::Gamepad_RightY))
        {
            // Invert the value because positive Y means stick down
            float ScrollValue = -InAnalogInputEvent.GetAnalogValue();
            ScrollTutorialContent(ScrollValue);
            return FReply::Handled();
        }
    }

    return Super::NativeOnAnalogValueChanged(InGeometry, InAnalogInputEvent);
}



FNavigationReply UMain_Menu_Widget::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    EUINavigation Direction = InNavigationEvent.GetNavigationType();

    // Handle Master Audio navigation
    if (MasterAudioButton && MasterAudioButton->HasKeyboardFocus())
    {
        if (Direction == EUINavigation::Left)
        {
            AdjustMasterVolume(false);
            return FNavigationReply::Explicit(nullptr);
        }
        else if (Direction == EUINavigation::Right)
        {
            AdjustMasterVolume(true);
            return FNavigationReply::Explicit(nullptr);
        }
    }

    // Play sound for navigation keys
    if (Direction == EUINavigation::Left ||
        Direction == EUINavigation::Right ||
        Direction == EUINavigation::Up ||
        Direction == EUINavigation::Down)
    {
        PlayNavigationSound();
    }


    return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
}




FReply UMain_Menu_Widget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Check if we're on the title screen
    if (bIsOnTitleScreen)
    {
        // Any mouse button will move to main menu
        SwitchToMainMenu();
        PlayNavigationSound(); // Optional - play a sound when transitioning
      //  GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Mouse button pressed"));
        return FReply::Handled();
    }
    else
    {
        // Not on title screen - check for right-click for back functionality
        if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
        {
            HandleGoBack();
         //   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Back with Right-Click!"));
            PlayBackSound();
            return FReply::Handled();
        }
    }

    // For other screens, default to regular handling
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}




void UMain_Menu_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);


    SwitchToWeaponSelectMenu();
    UpdateCharacterImage();



    // Add this debug code to see controller status (remove after testing)
    static bool bLastControllerState = false;
    static float TimeSinceLastCheck = 0.0f;
    TimeSinceLastCheck += InDeltaTime;

    // Check every second
    if (TimeSinceLastCheck > 1.0f)
    {
        bool bCurrentState = IsControllerConnected();
        if (bCurrentState != bLastControllerState)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f,
                bCurrentState ? FColor::Green : FColor::Red,
                FString::Printf(TEXT("Controller Connected: %s"),
                    bCurrentState ? TEXT("YES") : TEXT("NO")));
            bLastControllerState = bCurrentState;
        }
        TimeSinceLastCheck = 0.0f;
    }





    // Check if the sword button has keyboard focus and update stats
    if (SwordButton && SwordButton->HasKeyboardFocus())
    {
        UpdateWeaponStats(EWeaponType::Sword);
        OnWeaponButtonHovered(LOCTEXT("SwordDescAttack", "A sharp sword with enhanced attack power.").ToString());

    }

    else if (SwordButton && SwordButton->IsHovered())

    {
        UpdateWeaponStats(EWeaponType::Sword);
    }


    // Check if the staff button has keyboard focus and update stats
    if (StaffButton && StaffButton->HasKeyboardFocus())
    {
        UpdateWeaponStats(EWeaponType::Staff);
        OnWeaponButtonHovered(LOCTEXT("StaffDescBoost", "A mystical staff that boosts elemental power.").ToString());

    }


    else if (StaffButton && StaffButton->IsHovered())

    {
        UpdateWeaponStats(EWeaponType::Staff);

    }


    if (ControlsButton && ControlsButton->HasKeyboardFocus())
    {
        OnControlsButtonHovered();
    }

    else if (GameplayButton && GameplayButton->HasKeyboardFocus())
    {
        OnGameplayButtonHovered();
    }

    else if (LevellingUpButton && LevellingUpButton->HasKeyboardFocus())
    {
        OnLevellingUpButtonHovered();
    }

    else if (AttributesButton && AttributesButton->HasKeyboardFocus())
    {
        OnAttributesButtonHovered();
    }


}



ARen_Low_Poly_Character* UMain_Menu_Widget::GetPlayerCharacter()
{
    return nullptr;
}



void UMain_Menu_Widget::CheckAndApplyDemoRestrictions()
{

    if (bIsDemoBuild && StaffButton)

    {

        StaffButton->SetIsEnabled(false);
        StaffButton->RenderOpacity = 0.5f;

        if (StaffImage)

        {
            StaffImage->SetRenderOpacity(0.3f);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("This is the demo version"));
        }
    }

}


#undef LOCTEXT_NAMESPACE
