// Fill out your copyright notice in the Description page of Project Settings.


#include "Elemental_Attacks_List_Widget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Elemental_Attacks_Button_Widget.h"
#include "Test_Button_Widget.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"



void UElemental_Attacks_List_Widget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    
}



void UElemental_Attacks_List_Widget::SetupInputMode()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        //InputMode.SetHideCursorDuringCapture(false);
        PlayerController->SetInputMode(InputMode);
    }
}



void UElemental_Attacks_List_Widget::NativeConstruct()
{

    Super::NativeConstruct();


   // NewPopulateElementalAttackList();
    
    

     // Get the player character
    ARen_Low_Poly_Character* Character = Cast<ARen_Low_Poly_Character>(GetOwningPlayerPawn());

    if (Character)
    {
        SetupWidget(Character);
        UE_LOG(LogTemp, Log, TEXT("SetupWidget called from NativeConstruct"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get player character in NativeConstruct"));
    }
 
 
}




void UElemental_Attacks_List_Widget::SetupWidget(ARen_Low_Poly_Character* Character)
{
    PlayerCharacter = Character;
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerCharacter passed to SetupWidget"));
        return;
    }

    if (Elemental_Attack_ScrollBox)
    {
        // Check if this is a loaded game
        bool bIsLoadedGame = PlayerCharacter->bIsGameLoaded;

        if (bIsLoadedGame)
        {
            // Add a delay before populating the list
            FTimerHandle PopulateTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                PopulateTimerHandle,
                this,
                &UElemental_Attacks_List_Widget::PopulateElementalAttackList,
                0.5f,  // 0.5 second delay
                false
            );
        }
        else
        {
            // Normal game start, populate immediately
            PopulateElementalAttackList();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Elemental_Attack_ScrollBox is not valid in SetupWidget"));
    }
}




void UElemental_Attacks_List_Widget::PopulateElementalAttackList()
{


    if (!Elemental_Attack_ScrollBox || !PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Scroll box or PlayerCharacter is null!"));
        return;
    }

    // Clear any existing children
    Elemental_Attack_ScrollBox->ClearChildren();
    TArray<UElemental_Attacks_Button_Widget*> CreatedButtons;

    // Get the current weapon type from the player character
    EWeaponType CurrentWeaponType = PlayerCharacter->WeaponType;

    UE_LOG(LogTemp, Error, TEXT("=== POPULATING ELEMENTAL ATTACKS UI ==="));
    UE_LOG(LogTemp, Error, TEXT("Current Weapon Type: %s"), *UEnum::GetValueAsString(CurrentWeaponType));

    // Check if the weapon type exists in the elemental attacks map
    if (PlayerCharacter->WeaponElementalAttacks.Contains(CurrentWeaponType))
    {
        const FWeaponElementalAttacks& WeaponAttacks = PlayerCharacter->WeaponElementalAttacks[CurrentWeaponType];

        // Get the current proficiency levels
        const FElemental_Proficiency_Struct& ProficiencyStruct =
            PlayerCharacter->WeaponElementalProficiency.ElementalWeaponProficiencyMap[CurrentWeaponType];

        UE_LOG(LogTemp, Error, TEXT("Current Proficiency Levels:"));
        UE_LOG(LogTemp, Error, TEXT("  Fire Level: %d"), ProficiencyStruct.FireLevel);
        UE_LOG(LogTemp, Error, TEXT("  Ice Level: %d"), ProficiencyStruct.IceLevel);
        UE_LOG(LogTemp, Error, TEXT("  Thunder Level: %d"), ProficiencyStruct.ThunderLevel);

        // Create elemental attack buttons
        for (int32 Index = 0; Index < WeaponAttacks.ElementalAttacks.Num(); ++Index)
        {
            const FElemental_Struct& ElementalAttack = WeaponAttacks.ElementalAttacks[Index];

            UE_LOG(LogTemp, Error, TEXT("Checking Attack: %s"), *ElementalAttack.ElementalAttackName);
            UE_LOG(LogTemp, Error, TEXT("  Level: %d"), ElementalAttack.ElementalLevel);
            UE_LOG(LogTemp, Error, TEXT("  Type: %s"), *UEnum::GetValueAsString(ElementalAttack.ElementalType));
            UE_LOG(LogTemp, Error, TEXT("  Is Unlocked: %s"), ElementalAttack.bIsUnlocked ? TEXT("TRUE") : TEXT("FALSE"));

            // MODIFIED: Explicitly check for attacks that should be displayed
            bool bShouldDisplay = false;
            switch (ElementalAttack.ElementalType)
            {
            case EElementalAttackType::Fire:
                bShouldDisplay = ElementalAttack.ElementalLevel <= ProficiencyStruct.FireLevel;
                break;
            case EElementalAttackType::Ice:
                bShouldDisplay = ElementalAttack.ElementalLevel <= ProficiencyStruct.IceLevel;
                break;
            case EElementalAttackType::Thunder:
                bShouldDisplay = ElementalAttack.ElementalLevel <= ProficiencyStruct.ThunderLevel;
                break;
            }

            UE_LOG(LogTemp, Error, TEXT("  Should Display: %s"), bShouldDisplay ? TEXT("TRUE") : TEXT("FALSE"));

            if ((bShouldDisplay || ElementalAttack.bIsUnlocked) && ElementalAttackButtonClass)
            {
                UElemental_Attacks_Button_Widget* ElementalButton =
                    CreateWidget<UElemental_Attacks_Button_Widget>(GetWorld(), ElementalAttackButtonClass);

                if (ElementalButton)
                {
                    // Explicitly set the parent list
                    ElementalButton->SetParentList(this);
                    ElementalButton->SetupButton(ElementalAttack, PlayerCharacter, Index);

                    if (ElementalButton->Elemental_Attack_Button)
                    {
                        ElementalButton->Elemental_Attack_Button->SetIsEnabled(true);
                        ElementalButton->Elemental_Attack_Button->IsFocusable = true;
                    }

                    Elemental_Attack_ScrollBox->AddChild(ElementalButton);
                    CreatedButtons.Add(ElementalButton);

                    UE_LOG(LogTemp, Error, TEXT("ADDED TO UI: %s (Level %d)"),
                        *ElementalAttack.ElementalAttackName,
                        ElementalAttack.ElementalLevel);
                }
            }
        }

        UE_LOG(LogTemp, Error, TEXT("Total Buttons Created: %d"), CreatedButtons.Num());

        // Set focus to first button with delay
        if (CreatedButtons.Num() > 0)
        {
            FTimerHandle FocusTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                FocusTimerHandle,
                [FirstButton = CreatedButtons[0]]()
                {
                    if (FirstButton && FirstButton->Elemental_Attack_Button)
                    {
                        FirstButton->Elemental_Attack_Button->SetKeyboardFocus();
                        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Focus set on first button"));
                    }
                },
                0.1f,
                    false
                    );
        }
    }
}




void UElemental_Attacks_List_Widget::NewPopulateElementalAttackList()
{
    if (!Elemental_Attack_ScrollBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("Scroll box is null!"));
        return;
    }

    // Clear any existing children
    Elemental_Attack_ScrollBox->ClearChildren();

    // Create three test buttons
    UTest_Button_Widget* FirstButton = CreateWidget<UTest_Button_Widget>(GetWorld(), Test_Button_Widget_Class);
    if (FirstButton)
    {
        Elemental_Attack_ScrollBox->AddChild(FirstButton);

        // Set focus after delay
        FTimerHandle FocusTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            FocusTimerHandle,
            [FirstButton]()
            {
                if (FirstButton && FirstButton->TestButton)
                {
                    FirstButton->TestButton->SetKeyboardFocus();
                    UE_LOG(LogTemp, Log, TEXT("Focus set on first test button"));
                }
            },
            0.001f,  // 0.1 second delay
                false  // Don't loop
                );

        UE_LOG(LogTemp, Log, TEXT("First test button added to scroll box"));
    }

    UTest_Button_Widget* SecondButton = CreateWidget<UTest_Button_Widget>(GetWorld(), Test_Button_Widget_Class);
    if (SecondButton)
    {
        Elemental_Attack_ScrollBox->AddChild(SecondButton);
        UE_LOG(LogTemp, Log, TEXT("Second test button added to scroll box"));
    }

    UTest_Button_Widget* ThirdButton = CreateWidget<UTest_Button_Widget>(GetWorld(), Test_Button_Widget_Class);
    if (ThirdButton)
    {
        Elemental_Attack_ScrollBox->AddChild(ThirdButton);
        UE_LOG(LogTemp, Log, TEXT("Third test button added to scroll box"));
    }
}








void UElemental_Attacks_List_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);



}


