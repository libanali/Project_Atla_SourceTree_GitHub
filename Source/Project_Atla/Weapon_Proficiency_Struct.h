// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ren_Low_Poly_Character.h"
#include "Weapon_Proficiency_Struct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FWeapon_Proficiency_Struct
{
public:

    GENERATED_BODY()

        // Current experience points for the weapon
        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float CurrentExp;

    // Current level of the weapon
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 WeaponLevel;

    // Experience required to reach the next level
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ExpToNextLevel;

    FWeapon_Proficiency_Struct()
        : CurrentExp(0), WeaponLevel(1), ExpToNextLevel(100) {}
};
