// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elemental_Attack_Type.h"  // Include the enum header
#include "Elemental_Struct.generated.h"



USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FElemental_Struct
{
    GENERATED_BODY()

public:

    FElemental_Struct()
        : ElementalName(TEXT("Unknown Element")), ElementalType(EElementalAttackType::Fire),
        DamageMultiplier(1.0f), ManaCost(10.0f), ElementalLevel(1), bIsUnlocked(false) {}

    // Parameterized constructor
    FElemental_Struct(FString Name, EElementalAttackType Type, float InDamageMultiplier, float InManaCost,
        int32 InElementalLevel, bool bUnlocked)
        : ElementalName(Name), ElementalType(Type), DamageMultiplier(InDamageMultiplier),
        ManaCost(InManaCost), ElementalLevel(InElementalLevel), bIsUnlocked(bUnlocked) {}

    // Name of the elemental attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        FString ElementalName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        EElementalAttackType ElementalType;

    // The base damage multiplier for this elemental attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        float DamageMultiplier;

    // Mana cost for casting the attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        float ManaCost;

    // Current level of this elemental attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        int32 ElementalLevel;

    // Whether the elemental attack is unlocked or not
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        bool bIsUnlocked;

    // Description of the elemental attack (optional)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        FString ElementDescription;
};