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

    // Get the current weapon type from the player character
    EWeaponType CurrentWeaponType = PlayerCharacter->WeaponType;

    // Check if the weapon type exists in the elemental attacks map
    if (PlayerCharacter->WeaponElementalAttacks.Contains(CurrentWeaponType))
    {
        const FWeaponElementalAttacks& WeaponAttacks = PlayerCharacter->WeaponElementalAttacks[CurrentWeaponType];
        const FElemental_Proficiency_Struct& ProficiencyStruct =
            PlayerCharacter->WeaponElementalProficiency.ElementalWeaponProficiencyMap[CurrentWeaponType];

        // Create a set to track which attacks we've already added
        TSet<FString> AddedAttacks;

        // Sort attacks by level first, then by element type
        TArray<FElemental_Struct> SortedAttacks = WeaponAttacks.ElementalAttacks;
        SortedAttacks.Sort([](const FElemental_Struct& A, const FElemental_Struct& B) {
            if (A.ElementalLevel != B.ElementalLevel)
                return A.ElementalLevel < B.ElementalLevel;
            return A.ElementalType < B.ElementalType;
            });

        // Create buttons for unique attacks
        for (const FElemental_Struct& ElementalAttack : SortedAttacks)
        {
            // Create a unique key for this attack
            FString AttackKey = FString::Printf(TEXT("%s_%d_%d"),
                *ElementalAttack.ElementalAttackName,
                static_cast<int32>(ElementalAttack.ElementalType),
                ElementalAttack.ElementalLevel);

            // Skip if we've already added this attack
            if (AddedAttacks.Contains(AttackKey))
            {
                UE_LOG(LogTemp, Warning, TEXT("Skipping duplicate attack: %s"), *AttackKey);
                continue;
            }

            if (ElementalAttackButtonClass)
            {
                UElemental_Attacks_Button_Widget* ElementalButton =
                    CreateWidget<UElemental_Attacks_Button_Widget>(GetWorld(), ElementalAttackButtonClass);

                if (ElementalButton)
                {
                    // Add to our set of tracked attacks
                    AddedAttacks.Add(AttackKey);

                    ElementalButton->SetParentList(this);
                    ElementalButton->SetupButton(ElementalAttack, PlayerCharacter, 0);

                    // Determine if the button should be enabled based on proficiency level
                    bool bShouldEnable = false;
                    switch (ElementalAttack.ElementalType)
                    {
                    case EElementalAttackType::Fire:
                        bShouldEnable = ElementalAttack.ElementalLevel <= ProficiencyStruct.FireLevel;
                        break;
                    case EElementalAttackType::Ice:
                        bShouldEnable = ElementalAttack.ElementalLevel <= ProficiencyStruct.IceLevel;
                        break;
                    case EElementalAttackType::Thunder:
                        bShouldEnable = ElementalAttack.ElementalLevel <= ProficiencyStruct.ThunderLevel;
                        break;
                    }

                    // Update both the button's enabled state AND its visual state
                    if (ElementalButton->Elemental_Attack_Button)
                    {
                        if (!bShouldEnable)
                        {
                            // Locked (not high enough elemental level)
                            ElementalButton->Elemental_Attack_Button->SetIsEnabled(false);
                            ElementalButton->Elemental_Attack_Button->IsFocusable = false;
                            ElementalButton->SetRenderOpacity(0.5f);
                            ElementalButton->SetButtonTextColor(FLinearColor::White);  // Normal color
                        }
                        else if (bShouldEnable &&
                            PlayerCharacter->ManaStruct.CurrentMana < ElementalAttack.ManaCost)
                        {
                            // Unlocked but not enough mana
                            ElementalButton->Elemental_Attack_Button->SetIsEnabled(true);
                            ElementalButton->Elemental_Attack_Button->IsFocusable = true;
                            ElementalButton->SetRenderOpacity(0.55f);
                            ElementalButton->SetButtonTextColor(FLinearColor::Red);  // Red color
                        }
                        else
                        {
                            // Unlocked and enough mana
                            ElementalButton->Elemental_Attack_Button->SetIsEnabled(true);
                            ElementalButton->Elemental_Attack_Button->IsFocusable = true;
                            ElementalButton->SetRenderOpacity(1.0f);
                            ElementalButton->SetButtonTextColor(FLinearColor::White);  // Normal color
                        }
                    }

                    Elemental_Attack_ScrollBox->AddChild(ElementalButton);

                    UE_LOG(LogTemp, Warning, TEXT("Added Attack: %s (Level %d, Enabled: %s)"),
                        *ElementalAttack.ElementalAttackName,
                        ElementalAttack.ElementalLevel,
                        bShouldEnable ? TEXT("TRUE") : TEXT("FALSE"));
                }
            }
        }

        // Set focus to first button with delay
        if (Elemental_Attack_ScrollBox->GetChildrenCount() > 0)
        {
            FTimerHandle FocusTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                FocusTimerHandle,
                [this]()
                {
                    if (Elemental_Attack_ScrollBox->GetChildAt(0))
                    {
                        UElemental_Attacks_Button_Widget* FirstButton =
                            Cast<UElemental_Attacks_Button_Widget>(Elemental_Attack_ScrollBox->GetChildAt(0));

                        if (FirstButton && FirstButton->Elemental_Attack_Button)
                        {
                            FirstButton->Elemental_Attack_Button->SetKeyboardFocus();
                        }
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


