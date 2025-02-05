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
        WeaponElementalProficiency.ElementalWeaponProficiencyMap = LoadGameInstance->SavedElementalProficiencyMap;
        WeaponElementalAttacks = LoadGameInstance->SavedWeaponElementalAttacks;

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
        // Initialize default proficiency levels for new game
        FElemental_Proficiency_Struct DefaultProficiency;
        DefaultProficiency.FireLevel = 1;
        DefaultProficiency.IceLevel = 1;
        DefaultProficiency.ThunderLevel = 1;

        DefaultProficiency.FireProficiencyThresholds.Add(1, 100.f);
        DefaultProficiency.IceProficiencyThresholds.Add(1, 100.f);
        DefaultProficiency.ThunderProficiencyThresholds.Add(1, 100.f);

        WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Sword, DefaultProficiency);
        WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Staff, DefaultProficiency);

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
        OutAttack = 8.0f;
        OutDefense = 5.0f;
        OutElementalAttack = 4.0f;
        OutWeaponLevel = 1;  // Default level for Sword

    }

    else if (WeaponType == EWeaponType::Staff)
    {
        OutAttack = 4.0f;
        OutDefense = 3.0f;
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

bool UGame_Instance::GetElementalProficiencyLevels(EWeaponType WeaponType, int32& OutFireLevel, int32& OutIceLevel, int32& OutThunderLevel) const
{

    OutFireLevel = 1;
    OutIceLevel = 1;
    OutThunderLevel = 1;

    if (WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(WeaponType))
    {
        const FElemental_Proficiency_Struct& ProficiencyStruct = WeaponElementalProficiency.ElementalWeaponProficiencyMap[WeaponType];
        OutFireLevel = ProficiencyStruct.FireLevel;
        OutIceLevel = ProficiencyStruct.IceLevel;
        OutThunderLevel = ProficiencyStruct.ThunderLevel;
        return true;
    }
    return false;
}


