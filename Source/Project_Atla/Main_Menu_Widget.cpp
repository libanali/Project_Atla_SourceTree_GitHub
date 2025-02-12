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
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Game_Instance.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player_Save_Game.h"
#include "Carousel_Button_Widget.h"
#include "Components/AudioComponent.h"



void UMain_Menu_Widget::NativeConstruct()
{

    Super::NativeConstruct();


    InitializeCanvasPanels();
    UpdateCanvasVisibility(0);


    // Initialize panels and visibility
    InitializeCanvasPanels();
    UpdateCanvasVisibility(0);

    // Load saved settings first
    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        GameInstance->LoadSettings();
        GameInstance->LoadPlayerProgress();
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
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                FString::Printf(TEXT("Playing Music at Volume: %.2f"), InitialVolume));
        }
    }

    // Initialize settings controls
    InitializeSettingsControls();

    // Initialize menu buttons
    InitializeMenuButtons();

    // Set widget as focusable
    this->SetIsFocusable(true);
    bIsOnTitleScreen = true;
    bHasSetFocusForSwordButton = false;




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

        // Language Toggle setup
        if (LanguageToggle)
        {
            LanguageToggle->PossibleValues = { "English", "French", "German", "Spanish", "Japanese" };
            LanguageToggle->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnLanguageValueChanged);
            LanguageToggle->SetLabel("Language");

            int32 LangIndex = LanguageToggle->PossibleValues.Find(GameInstance->GameSettings.CurrentLanguage);
            LanguageToggle->CurrentIndex = (LangIndex != INDEX_NONE) ? LangIndex : 0;
            LanguageToggle->UpdateDisplay();
        }

        // Screen Shake Toggle setup
        if (ScreenShakeToggle)
        {
            ScreenShakeToggle->PossibleValues = { "ON", "OFF" };
            ScreenShakeToggle->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnScreenShakeValueChanged);
            ScreenShakeToggle->SetLabel("Screen Shake");
            ScreenShakeToggle->CurrentIndex = GameInstance->GameSettings.bScreenShakeEnabled ? 0 : 1;
            ScreenShakeToggle->UpdateDisplay();
        }

        // Vibration Toggle setup
        if (VibrationToggle)
        {
            VibrationToggle->PossibleValues = { "ON", "OFF" };
            VibrationToggle->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnVibrationValueChanged);
            VibrationToggle->SetLabel("Vibration");
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

        // Set focus on SwordButton instead of PlayButton since we're in weapon select
        if (SwordButton)
        {
            SwordButton->SetKeyboardFocus();
            LastFocusedButton = PlayButton;
        }
    }

}



void UMain_Menu_Widget::OnTutorialClicked()
{

    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(5);
        UpdateCanvasVisibility(5);

        // Set focus on SwordButton instead of PlayButton since we're in weapon select
        if (ControlsButton)
        {
            ControlsButton->SetKeyboardFocus();
            LastFocusedButton = TutorialButton;
        }
    }


}





void UMain_Menu_Widget::OnSettingsClicked()
{

    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidgetIndex(4); // Settings is index 4
        UpdateCanvasVisibility(4);
        if (SettingsButton)
        {
            MasterAudioButton->SetKeyboardFocus();
            LastFocusedButton = SettingsButton;
        }
    }

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
    UGameplayStatics::OpenLevel(this, FName("LowPoly_Level"));


}



void UMain_Menu_Widget::OnSwordButtonHovered()
{

    UpdateWeaponStats(EWeaponType::Sword);
    OnWeaponButtonHovered(TEXT("A sharp sword with enhanced lightning power."));
    GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Sword Button Hovered"));

    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());

    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null in OnSwordButtonHovered!"));
        return;
    }

    // Check if the weapon data exists in the proficiency map
    if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Sword))
    {
        const FWeapon_Proficiency_Struct& SwordStats = GameInstance->WeaponProficiencyMap[EWeaponType::Sword];

        // Update the weapon level text
        if (WeaponLevelStat)
        {
            WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordStats.WeaponLevel)));
            UE_LOG(LogTemp, Log, TEXT("Updated WeaponLevelStat with Sword Level: %d"), SwordStats.WeaponLevel);
        }

        // Update elemental levels
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

            UE_LOG(LogTemp, Log, TEXT("Updated Sword Elemental Levels - Fire: %d, Ice: %d, Thunder: %d"),
                ElementalStats.FireLevel, ElementalStats.IceLevel, ElementalStats.ThunderLevel);
        }
    }


    if (CharacterWeaponRender && SwordCharacterTexture)

    {

        CharacterWeaponRender->SetBrushFromTexture(SwordCharacterTexture);

    }
}



void UMain_Menu_Widget::OnSwordButtonFocused()
{

    UpdateWeaponStats(EWeaponType::Sword);
    OnWeaponButtonHovered(TEXT("A sharp sword with enhanced lightning power."));
    GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Sword Button Focused"));
    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());

    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null in OnSwordButtonHovered!"));
        return;
    }

    // Check if the weapon data exists in the proficiency map
    if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Sword))
    {
        const FWeapon_Proficiency_Struct& SwordStats = GameInstance->WeaponProficiencyMap[EWeaponType::Sword];

        // Update the weapon level text
        if (WeaponLevelStat)
        {
            WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), SwordStats.WeaponLevel)));
            UE_LOG(LogTemp, Log, TEXT("Updated WeaponLevelStat with Sword Level: %d"), SwordStats.WeaponLevel);
        }
       

        // Update elemental levels
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

            UE_LOG(LogTemp, Log, TEXT("Updated Sword Elemental Levels - Fire: %d, Ice: %d, Thunder: %d"),
                ElementalStats.FireLevel, ElementalStats.IceLevel, ElementalStats.ThunderLevel);
        }
    }


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
    UGameplayStatics::OpenLevel(this, FName("LowPoly_Level"));



}



void UMain_Menu_Widget::OnStaffButtonHovered()
{

    UpdateWeaponStats(EWeaponType::Staff);
    OnWeaponButtonHovered(TEXT("A mystical staff that boosts elemental power."));
    GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Staff Button Hovered"));

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


    if (CharacterWeaponRender && StaffCharacterTexture)

    {

        CharacterWeaponRender->SetBrushFromTexture(StaffCharacterTexture);

    }

}



void UMain_Menu_Widget::OnStaffButtonFocused()
{


    UpdateWeaponStats(EWeaponType::Staff);
    OnWeaponButtonHovered(TEXT("A mystical staff that boosts elemental power."));
    GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Staff Button Focused"));


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

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Controls Button Hovered"));

    // Update title and description
    UpdateTutorialContent(
        FText::FromString("Controls"),
        FText::FromString("")
    );

    // Show the controls container
    if (ControlsContainer)
    {
        ControlsContainer->SetVisibility(ESlateVisibility::Visible);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Controls Container Shown"));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Controls Container is null!"));
    }
}




void UMain_Menu_Widget::OnControlsButtonUnhovered()
{

    // Hide the container when no longer hovering
    if (ControlsContainer)
    {
        ControlsContainer->SetVisibility(ESlateVisibility::Hidden);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Controls Container Hidden"));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Controls Container is null!"));
    }

    // Clear the title and description when unhovered
    UpdateTutorialContent(
        FText::FromString(""),
        FText::FromString("")
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
        FText::FromString("Gameplay"),
        FText::FromString("Combat System:\n- Deal damage with weapons and magic\n- Block to reduce incoming damage\n- Use items to restore health\n\nExploration:\n- Find hidden treasures\n- Complete side quests\n- Discover new abilities")
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
        FText::FromString("Levelling Up"),
        FText::FromString("Experience Points:\n- Gain EXP from defeating enemies\n- Level up to increase stats\n- Unlock new abilities\n\nWeapon Proficiency:\n- Use weapons to increase mastery\n- Higher levels unlock weapon skills\n- Each weapon has unique abilities")
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
        FText::FromString("Attributes"),
        FText::FromString("Core Stats:\n- HP: Your health points\n- Attack: Physical damage dealt\n- Defense: Damage reduction\n- Magic: Spell power\n\nElemental Stats:\n- Fire: Fire magic power\n- Ice: Ice magic power\n- Thunder: Lightning power")
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
        VolumePercentageText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), VolumePercent)));
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

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
            FString::Printf(TEXT("Volume Changed: %.2f"), Value));
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
        AttackStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int32>(Attack))));
        UE_LOG(LogTemp, Warning, TEXT("AttackStat updated with: %d"), static_cast<int32>(Attack));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AttackStat is null!"));
    }

    if (DefenceStat)
    {
        // Convert Defense to an integer and update the text
        DefenceStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int32>(Defense))));
        UE_LOG(LogTemp, Warning, TEXT("DefenceStat updated with: %d"), static_cast<int32>(Defense));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DefenceStat is null!"));
    }

    if (ElementalStat)
    {
        // Convert ElementalAttack to an integer and update the text
        ElementalStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int32>(ElementalAttack))));
        UE_LOG(LogTemp, Warning, TEXT("ElementalStat updated with: %d"), static_cast<int32>(ElementalAttack));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ElementalStat is null!"));
    }

    if (WeaponLevelStat)
    {
        // WeaponLevel is already an integer, so no conversion needed
        WeaponLevelStat->SetText(FText::FromString(FString::Printf(TEXT("%d"), WeaponLevel)));
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
            FireProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), FireLevel)));
            UE_LOG(LogTemp, Warning, TEXT("FireProficiencyLevel updated with: %d"), FireLevel);
        }

        if (IceProficiencyLevel)
        {
            IceProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), IceLevel)));
            UE_LOG(LogTemp, Warning, TEXT("IceProficiencyLevel updated with: %d"), IceLevel);
        }

        if (ThunderProficiencyLevel)
        {
            ThunderProficiencyLevel->SetText(FText::FromString(FString::Printf(TEXT("%d"), ThunderLevel)));
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

            // Restore focus to the last focused button
            if (LastFocusedButton)
            {
                LastFocusedButton->SetKeyboardFocus();
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
                    FString::Printf(TEXT("Focus restored to: %s"), *LastFocusedButton->GetName()));
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
        UpdateCanvasVisibility(1); // Add this line
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Main Menu!"));
        LastFocusedButton = nullptr;

        if (PlayButton)

        {

            PlayButton->SetKeyboardFocus();

        }

        bIsOnTitleScreen = false;
    }


}





void UMain_Menu_Widget::SwitchToWeaponSelectMenu()
{

    // Check if we're on Widget 2 (index 3)
    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        if (CurrentIndex == 2 && !bHasSetFocusForSwordButton)
        {
            // Set focus on SwordButton if not already focused
            if (SwordButton)
            {
                SwordButton->SetKeyboardFocus();
                bHasSetFocusForSwordButton = true;  // Mark that focus has been set
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Sword button focused!"));
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
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
            FString::Printf(TEXT("Screen Shake: %s"), *NewValue));
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
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
            FString::Printf(TEXT("Vibration: %s"), *NewValue));
    }



}


void UMain_Menu_Widget::OnLanguageValueChanged(const FString& NewValue)
{


    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        // Update the game setting
        GameInstance->GameSettings.CurrentLanguage = NewValue;

        // Save settings
        //GameInstance->SaveSettings();

        // Debug message
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
            FString::Printf(TEXT("Language Changed to: %s"), *NewValue));
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

    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Settings Reset to Default"));


}




void UMain_Menu_Widget::OnApplyChangesClicked()
{

    if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
    {
        GameInstance->SaveSettings();
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Settings Applied"));

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
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Master Audio Button Focused"));
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



}






FReply UMain_Menu_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{


    // Get the pressed key
    FKey PressedKey = InKeyEvent.GetKey();

    if (PressedKey == EKeys::Gamepad_FaceButton_Right || PressedKey == EKeys::J || PressedKey == EKeys::RightMouseButton)
    {
        HandleGoBack(); // Handle the back button logic
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Back!"));
        PlayBackSound();
        return FReply::Handled();
    }


    // Check for "Any Key" or "Gamepad Face Button Bottom" for switching to the main menu
    if (PressedKey == EKeys::AnyKey || PressedKey == EKeys::LeftMouseButton || PressedKey == EKeys::Gamepad_FaceButton_Bottom || PressedKey == EKeys::Gamepad_FaceButton_Top || PressedKey == EKeys::Gamepad_FaceButton_Left || PressedKey == EKeys::Gamepad_FaceButton_Right || PressedKey == EKeys::Gamepad_Special_Right)
    {
        SwitchToMainMenu(); // Handle switching to the main menu
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


   


    // Optionally: Handle other keys here if needed
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);



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




void UMain_Menu_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);


    SwitchToWeaponSelectMenu();
    UpdateCharacterImage();

    // Check if the sword button has keyboard focus and update stats
    if (SwordButton && SwordButton->HasKeyboardFocus())
    {
        UpdateWeaponStats(EWeaponType::Sword);
        OnWeaponButtonHovered("A sharp sword with enhanced attack power.");

    }

    else if (SwordButton && SwordButton->IsHovered())

    {
        UpdateWeaponStats(EWeaponType::Sword);
    }


    // Check if the staff button has keyboard focus and update stats
    if (StaffButton && StaffButton->HasKeyboardFocus())
    {
        UpdateWeaponStats(EWeaponType::Staff);
        OnWeaponButtonHovered("A mystical staff that boosts elemental power.");
 
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


