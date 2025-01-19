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

    // Map of level thresholds (similar to elemental system)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        TMap<int32, float> WeaponProficiencyThresholds;

    // Stat boosts for each level
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        TMap<int32, float> AttackBoostPerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        TMap<int32, float> DefenseBoostPerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        TMap<int32, float> ElementalBoostPerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        TMap<int32, float> HealthBoostPerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        TMap<int32, float> ManaBoostPerLevel;

    // Current stat boosts (calculated based on level)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float AttackPowerBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float DefenseBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float ElementalPowerBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float MaxHealthBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
        float MaxManaBoost;

    // Constructor
    FWeapon_Proficiency_Struct()
        : WeaponLevel(1)
        , CurrentEXP(0.f)
        , AttackPowerBoost(0.f)
        , DefenseBoost(0.f)
        , ElementalPowerBoost(0.f)
        , MaxHealthBoost(0.f)
        , MaxManaBoost(0.0f)
    {
        // Initialize default thresholds
        WeaponProficiencyThresholds.Add(1, 100.f);
        WeaponProficiencyThresholds.Add(2, 200.f);
        WeaponProficiencyThresholds.Add(3, 300.f);

        // Initialize default stat boosts per level
        AttackBoostPerLevel.Add(1, 5.f);    // +5 attack at level 1
        AttackBoostPerLevel.Add(2, 10.f);   // +10 attack at level 2
        AttackBoostPerLevel.Add(3, 15.f);   // +15 attack at level 3

        DefenseBoostPerLevel.Add(1, 3.f);
        DefenseBoostPerLevel.Add(2, 6.f);
        DefenseBoostPerLevel.Add(3, 9.f);

        ElementalBoostPerLevel.Add(1, 5.f);
        ElementalBoostPerLevel.Add(2, 10.f);
        ElementalBoostPerLevel.Add(3, 15.f);

        HealthBoostPerLevel.Add(1, 10.f);
        HealthBoostPerLevel.Add(2, 20.f);
        HealthBoostPerLevel.Add(3, 30.f);

        ManaBoostPerLevel.Add(1, 10.f);
        ManaBoostPerLevel.Add(2, 20.f);
        ManaBoostPerLevel.Add(3, 30.f);
    }
};
