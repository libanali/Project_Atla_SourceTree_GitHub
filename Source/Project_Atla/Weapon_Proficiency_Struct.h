// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elemental_Struct.h"
#include "Elemental_Attack_Type.h" 
#include "Weapon_Proficiency_Struct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FWeapon_Proficiency_Struct
{
public:

    GENERATED_BODY()
        // Current level of the weapon
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        int32 WeaponLevel;

    // Current experience points (EXP) for this weapon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float CurrentEXP;

    // Experience points required to reach the next level
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float EXPToNextLevel;

    // Attack power boost provided by this weapon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float AttackPowerBoost;

    // Defense power boost provided by this weapon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float DefenseBoost;

    // Elemental damage boost provided by this weapon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float ElementalPowerBoost;

    // Max health boost provided by this weapon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float MaxHealthBoost;

    // Max health boost provided by this weapon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float MaxManaBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
    TMap<EElementalAttackType, FElemental_Struct> ElementalProficiencyMap;

    // Constructor to initialize default values
    FWeapon_Proficiency_Struct()
        : WeaponLevel(1),
        CurrentEXP(0.f),
        EXPToNextLevel(100.f),  // Default starting EXP for level 1
        AttackPowerBoost(0.f),
        DefenseBoost(0.f),
        ElementalPowerBoost(0.f),
        MaxHealthBoost(0.f),
        MaxManaBoost(0.0f)
    {
    }

};
