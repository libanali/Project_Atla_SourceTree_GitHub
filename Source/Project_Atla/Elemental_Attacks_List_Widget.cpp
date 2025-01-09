// Fill out your copyright notice in the Description page of Project Settings.


#include "Elemental_Attacks_List_Widget.h"
#include "Components/ScrollBox.h"
#include "Elemental_Attacks_Button_Widget.h"
#include "Ren_Low_Poly_Character.h"


void UElemental_Attacks_List_Widget::NativeOnInitialized()
{

    Super::NativeOnInitialized();

    // Make sure the scroll box and player character are valid before populating
    if (Elemental_Attack_ScrollBox && PlayerCharacter)
    {
     //   PopulateElementalAttackList();
    }

    


}



void UElemental_Attacks_List_Widget::SetupWidget(ARen_Low_Poly_Character* Character)
{
    // Store a reference to the player character
    PlayerCharacter = Character;

    // If the scroll box is valid, populate the list
    if (Elemental_Attack_ScrollBox)
    {
        PopulateElementalAttackList();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Elemental_Attack_ScrollBox is not valid."));
    }


}



void UElemental_Attacks_List_Widget::PopulateElementalAttackList()
{
    // Clear the scroll box before adding new buttons
    Elemental_Attack_ScrollBox->ClearChildren();

    // Validate the player character
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is null in Elemental_Attack_List_Widget."));
        return;
    }

    // A variable to keep track of the first button
    UElemental_Attacks_Button_Widget* FirstButton = nullptr;

    // Iterate through the player's weapon types and their associated elemental attacks
    for (const auto& WeaponAttacksPair : PlayerCharacter->WeaponElementalAttacks)
    {
        const EWeaponType& WeaponType = WeaponAttacksPair.Key;
        const FWeaponElementalAttacks& WeaponAttacks = WeaponAttacksPair.Value;

        // Loop through the elemental attacks for this weapon type
        for (int32 Index = 0; Index < WeaponAttacks.ElementalAttacks.Num(); ++Index)
        {
            const FElemental_Struct& ElementalAttack = WeaponAttacks.ElementalAttacks[Index];

            // Check if the attack is unlocked
            if (ElementalAttack.bIsUnlocked)
            {
                // Ensure the button class is valid
                if (ElementalAttackButtonClass)
                {
                    // Create a new button widget dynamically
                    UElemental_Attacks_Button_Widget* ElementalButton = CreateWidget<UElemental_Attacks_Button_Widget>(GetWorld(), ElementalAttackButtonClass);

                    if (ElementalButton)
                    {
                        // Set up the button with the attack's data, player reference, and the index
                        ElementalButton->SetupButton(ElementalAttack, PlayerCharacter, Index);

                        // Add the button to the scroll box
                        Elemental_Attack_ScrollBox->AddChild(ElementalButton);

                        // If this is the first button, set it as the focusable one
                        if (!FirstButton)
                        {
                            FirstButton = ElementalButton;
                        }
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("ElementalAttackButtonClass is not set in Elemental_Attack_List_Widget."));
                }
            }
        }
    }

    // If there's a first button, set focus on it
    if (FirstButton)
    {
        FirstButton->SetKeyboardFocus();
    }
}



void UElemental_Attacks_List_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);



}
