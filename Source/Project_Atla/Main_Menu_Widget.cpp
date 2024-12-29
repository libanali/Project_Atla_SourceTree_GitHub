// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Components/TextBlock.h"
#include "Kismet/Gameplaystatics.h"
#include "Components/WidgetSwitcher.h"
#include "Game_Instance.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"


void UMain_Menu_Widget::NativeConstruct()
{

    // Bind buttons to functions
    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnPlayClicked);
    }


    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnBackClicked);
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



    if (PressAnyButtonText)

    {
        PlayAnimation(FadeAnimation, 1.0f, 0);
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
    

}






void UMain_Menu_Widget::OnPlayClicked()
{

    if (WidgetSwitcher)
    {
        // Switch to Weapon Select (index 2)
        WidgetSwitcher->SetActiveWidgetIndex(2);
    }


}




void UMain_Menu_Widget::OnBackClicked()
{

    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        // Logic to go back to the previous menu
        if (CurrentIndex > 0)
        {
            WidgetSwitcher->SetActiveWidgetIndex(CurrentIndex - 1);
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

}



void UMain_Menu_Widget::OnSwordButtonFocused()
{

    UpdateWeaponStats(EWeaponType::Sword);
    OnWeaponButtonHovered(TEXT("A sharp sword with enhanced lightning power."));
    GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Sword Button Focused"));


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

}



void UMain_Menu_Widget::OnStaffButtonFocused()
{


    UpdateWeaponStats(EWeaponType::Staff);
    OnWeaponButtonHovered(TEXT("A mystical staff that boosts elemental power."));
    GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Black, TEXT("Staff Button Focused"));


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

    // Get the player's weapon proficiency map (assumed to be in the character)
    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (PlayerCharacter)
    {
        const TMap<EWeaponType, FWeapon_Proficiency_Struct>& WeaponProficiencyMap = PlayerCharacter->WeaponProficiencyMap;

        // Get the stats for the selected weapon
        if (WeaponProficiencyMap.Contains(WeaponType))
        {
            const FWeapon_Proficiency_Struct& WeaponProficiency = WeaponProficiencyMap[WeaponType];

            // Update the UI text blocks with weapon stats
            if (AttackStat)

            {
             //   AttackStat->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), WeaponProficiency.AttackPowerBoost)));

            }

            if (DefenceStat)

            {
              //  DefenceStat->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), WeaponProficiency.DefenseBoost)));

            }

            if (ElementalStat)

            {
              //  ElementalStat->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), WeaponProficiency.ElementalPowerBoost)));

            }
        }
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

    // Check for specific keys (e.g., "Gamepad Face Button Right" for PS4/5 circle or "Escape" for keyboard back)
    if (PressedKey == EKeys::Gamepad_FaceButton_Right || PressedKey == EKeys::J)
    {
        HandleGoBack(); // Handle the back button logic
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Back!"));
        return FReply::Handled();
    }


    // Check for "Any Key" or "Gamepad Face Button Bottom" for switching to the main menu
    if (PressedKey == EKeys::SpaceBar || PressedKey == EKeys::Gamepad_FaceButton_Bottom)
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


