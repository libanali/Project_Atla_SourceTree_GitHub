// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ren_Low_Poly_Character.h"
#include "Elemental_Struct.generated.h"



USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FElemental_Struct
{
    GENERATED_BODY()

public:

 
    FElemental_Struct()
        : ElementalName(TEXT("Unknown Element")), ElementType(EElementalAttackType::Fire),
        DamageMultiplier(1.0f), ManaCost(10.0f), ElementalLevel(1), bIsUnlocked(false) {}

    // Parameterized constructor
    FElemental_Struct(FString Name, EElementType Type, float InDamageMultiplier, float InManaCost,
        int32 InElementalLevel, bool bUnlocked)
        : ElementalName(Name), DamageMultiplier(InDamageMultiplier),
        ManaCost(InManaCost), ElementalLevel(InElementalLevel), bIsUnlocked(bUnlocked) {}

    // Name of the elemental attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        FString ElementalName;

    // Element type (Fire, Ice, Thunder, etc.)
    // Element type (Fire, Ice, Thunder, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        EElementalAttackType ElementType;

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