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
#include "Game_Instance.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player_Save_Game.h"


void UMain_Menu_Widget::NativeConstruct()
{


    Super::NativeConstruct();


    InitializeCanvasPanels();
    UpdateCanvasVisibility(0);



    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UMain_Menu_Widget::OnMasterVolumeChanged);

        // Load initial value
        if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
        {
            MasterVolumeSlider->SetValue(GameInstance->GameSettings.MasterVolume);
            UpdateVolumeText(GameInstance->GameSettings.MasterVolume);
        }
    }


    // Get player controller and set focus
    if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
    {
        PlayerController->SetInputMode(FInputModeUIOnly());
        PlayerController->bShowMouseCursor = true;
        this->SetUserFocus(PlayerController);
    }



    // Bind buttons to functions
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



    if (PressAnyButtonText)

    {
        PlayAnimation(PressAnyButtonFadeAnimation, 1.0f, 0);
    }


    if (TitleCanvas && TitleCanvasAnimation)
    {

        PlayAnimation(TitleCanvasAnimation);
    }


   
   APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)


    {

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("player controller active!"));


    }


    this->SetIsFocusable(true);

//    SwitchToWeaponSelectMenu();
    
    bIsOnTitleScreen = true;
    bHasSetFocusForSwordButton = false;


    // Get the custom game instance
    UGame_Instance* GameInstance = Cast<UGame_Instance>(GetWorld()->GetGameInstance());
    if (GameInstance)
    {
        // Load the player progress
        GameInstance->LoadPlayerProgress();

        // Example: Use the loaded data to update the UI or set weapon selection
        if (GameInstance->WeaponProficiencyMap.Contains(EWeaponType::Sword))
        {
            const FWeapon_Proficiency_Struct& SwordStats = GameInstance->WeaponProficiencyMap[EWeaponType::Sword];
            UE_LOG(LogTemp, Log, TEXT("Sword Level: %d, Sword EXP: %.2f"), SwordStats.WeaponLevel, SwordStats.CurrentEXP);
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





void UMain_Menu_Widget::OnWeaponButtonHovered(const FString& Description)
{

    if (WeaponDescription)
    {
        WeaponDescription->SetText(FText::FromString(Description));
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
        GameInstance->SaveSettings();
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

        // Only go back if the current index is greater than 1
        if (CurrentIndex > 1)
        {
            WidgetSwitcher->SetActiveWidgetIndex(CurrentIndex - 1);
            UpdateCanvasVisibility(CurrentIndex - 1); // Add this line

            // Check if switching back to Main Menu (index 1)
            if (WidgetSwitcher->GetActiveWidgetIndex() == 1 && PlayButton)
            {
                PlayButton->SetKeyboardFocus();
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Focused on Play Button"));
            }
        }
        else
        {
            // Optionally log or handle the case when trying to go back from the main menu
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






FReply UMain_Menu_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{


    // Get the pressed key
    FKey PressedKey = InKeyEvent.GetKey();

    if (PressedKey == EKeys::Gamepad_FaceButton_Right || PressedKey == EKeys::J || PressedKey == EKeys::RightMouseButton)
    {
        HandleGoBack(); // Handle the back button logic
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Back!"));
        return FReply::Handled();
    }


    // Check for "Any Key" or "Gamepad Face Button Bottom" for switching to the main menu
    if (PressedKey == EKeys::AnyKey || PressedKey == EKeys::LeftMouseButton || PressedKey == EKeys::Gamepad_FaceButton_Bottom || PressedKey == EKeys::Gamepad_FaceButton_Top || PressedKey == EKeys::Gamepad_FaceButton_Left || PressedKey == EKeys::Gamepad_FaceButton_Right || PressedKey == EKeys::Gamepad_Special_Right)
    {
        SwitchToMainMenu(); // Handle switching to the main menu
        return FReply::Handled();
    }


    // Optionally: Handle other keys here if needed
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);



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

}

ARen_Low_Poly_Character* UMain_Menu_Widget::GetPlayerCharacter()
{
    return nullptr;
}


