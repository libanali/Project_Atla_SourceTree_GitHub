// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Instance.h"
#include "Player_Save_Game.h"
#include "Kismet/GameplayStatics.h"

void UGame_Instance::LoadPlayerProgress()
{


    UPlayer_Save_Game* LoadGameInstance = Cast<UPlayer_Save_Game>(UGameplayStatics::LoadGameFromSlot(TEXT("Player Save Slot"), 0));
    if (LoadGameInstance)
    {
        WeaponProficiencyMap = LoadGameInstance->SavedWeaponProficiencyMap;

        UE_LOG(LogTemp, Log, TEXT("Successfully loaded weapon proficiency map."));
        for (const TPair<EWeaponType, FWeapon_Proficiency_Struct>& Pair : WeaponProficiencyMap)
        {
            UE_LOG(LogTemp, Warning, TEXT("Loaded %s: Level %d, EXP %.2f"),
                *UEnum::GetValueAsString(Pair.Key),
                Pair.Value.WeaponLevel,
                Pair.Value.CurrentEXP);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game found. Initializing default values."));
    }


}

bool UGame_Instance::GetWeaponBaseStats(EWeaponType WeaponType, float& OutAttack, float& OutDefense, float& OutElementalAttack, int32& OutWeaponLevel) const
{
    // Initialize outputs
    OutAttack = 0.0f;
    OutDefense = 0.0f;
    OutElementalAttack = 0.0f;
    OutWeaponLevel = 0;

    // Retrieve base stats based on WeaponType
    if (WeaponType == EWeaponType::Sword)
    {
        OutAttack = 3.0f;
        OutDefense = 2.0f;
        OutElementalAttack = 4.0f;
        OutWeaponLevel = 1;  // Default level for Sword

    }

    else if (WeaponType == EWeaponType::Staff)
    {
        OutAttack = 20.0f;
        OutDefense = 2.0f;
        OutElementalAttack = 10.0f;
        OutWeaponLevel = 1;  // Default level for Sword

    }

    else

    {
        // If the weapon type is not recognized, return false
        return false;
    }

    // Check if the weapon exists in the proficiency map
    if (WeaponProficiencyMap.Contains(WeaponType))
    {
        const FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[WeaponType];

        // Apply proficiency boosts to the base stats
        OutAttack += Proficiency.AttackPowerBoost;
        OutDefense += Proficiency.DefenseBoost;
        OutElementalAttack += Proficiency.ElementalPowerBoost;
        OutWeaponLevel = Proficiency.WeaponLevel;
    }

    return true;
}


