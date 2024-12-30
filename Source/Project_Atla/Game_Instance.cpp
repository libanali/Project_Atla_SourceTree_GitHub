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

bool UGame_Instance::GetWeaponBaseStats(EWeaponType WeaponType, float& OutAttack, float& OutDefense, float& OutElementalAttack) const
{
    // Initialize outputs to 0
    OutAttack = 0.0f;
    OutDefense = 0.0f;
    OutElementalAttack = 0.0f;

    // Return base stats based on WeaponType
    if (WeaponType == EWeaponType::Sword)
    {
        OutAttack = 3.0f;
        OutDefense = 2.0f;
        OutElementalAttack = 4.0f;
        return true;
    }
    else if (WeaponType == EWeaponType::Staff)
    {
        OutAttack = 20.0f;
        OutDefense = 2.0f;
        OutElementalAttack = 10.0f;
        return true;
    }

    // If the weapon type is not recognized
    return false;
}


