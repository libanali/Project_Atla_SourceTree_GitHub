// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementalEXPManager.h"
#include "Ren_Low_Poly_Character.h"
#include "Logging/LogMacros.h"




void UElementalEXPManager::InitializeEXPManager(TArray<FElemental_Struct>& InElementalAttacks)
{


	ElementalAttacks = &InElementalAttacks;



}

void UElementalEXPManager::GainEXP(int32 ElementalIndex, float EXPToGain)
{

    if (ElementalIndex >= 0 && ElementalIndex < ElementalAttacks->Num())
    {
        FElemental_Struct& SelectedElementalAttack = (*ElementalAttacks)[ElementalIndex];

        // Log before gaining EXP
        UE_LOG(LogTemp, Warning, TEXT("Elemental Attack %s gained %.2f EXP. Current EXP: %.2f / %.2f"),
            *SelectedElementalAttack.ElementalAttackName,
            EXPToGain,
            SelectedElementalAttack.CurrentEXP,
            SelectedElementalAttack.EXPToNextLevel);

        // Add EXP to the elemental attack
        SelectedElementalAttack.CurrentEXP += EXPToGain;

        // After gaining EXP, check if leveling up is required
        CheckLevelUp(ElementalIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid ElementalIndex %d for ElementalAttacks array."), ElementalIndex);
    }




}

void UElementalEXPManager::CheckLevelUp(int32 ElementalIndex)
{



    if (ElementalIndex >= 0 && ElementalIndex < ElementalAttacks->Num())
    {
        FElemental_Struct& SelectedElementalAttack = (*ElementalAttacks)[ElementalIndex];

        // Check if the elemental attack has enough EXP to level up
        while (SelectedElementalAttack.CurrentEXP >= SelectedElementalAttack.EXPToNextLevel)
        {
            SelectedElementalAttack.CurrentEXP -= SelectedElementalAttack.EXPToNextLevel;
            SelectedElementalAttack.ElementalLevel++;
            SelectedElementalAttack.EXPToNextLevel *= 1.5f; // Increase EXP needed for the next level

            // Log level-up event
            UE_LOG(LogTemp, Warning, TEXT("Elemental Attack %s leveled up! New Level: %d. EXP required for next level: %.2f"),
                *SelectedElementalAttack.ElementalAttackName,
                SelectedElementalAttack.ElementalLevel,
                SelectedElementalAttack.EXPToNextLevel);
        }
    }



}
