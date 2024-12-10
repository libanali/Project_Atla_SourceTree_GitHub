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
        : ElementalAttackName(TEXT("Unknown Element")),
        ElementalType(EElementalAttackType::Fire),
        DamageMultiplier(1.0f),
        ManaCost(10.0f),
        ElementalLevel(1),
        bIsUnlocked(false),
        CurrentEXP(0.0f),
        EXPToNextLevel(100.0f),
        Elemental_Attack_Animation(nullptr)
    {}

    FElemental_Struct(
        FString Name,
        EElementalAttackType Type,
        float InDamageMultiplier,
        float InManaCost,
        int32 InElementalLevel,
        bool bUnlocked,
        UAnimMontage* InAnimationMontage,
        float InEXP = 0.0f,
        float InEXPToNextLevel = 2000.0f
    )
        : ElementalAttackName(Name),
        ElementalType(Type),
        DamageMultiplier(InDamageMultiplier),
        ManaCost(InManaCost),
        ElementalLevel(InElementalLevel),
        bIsUnlocked(bUnlocked),
        CurrentEXP(InEXP),
        EXPToNextLevel(InEXPToNextLevel),
        Elemental_Attack_Animation(InAnimationMontage)
    {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        FString ElementalAttackName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        EElementalAttackType ElementalType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique Animation")
        UAnimMontage* Elemental_Attack_Animation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        float DamageMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        float ManaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        int32 ElementalLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        float CurrentEXP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        float EXPToNextLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        bool bIsUnlocked;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
        FString ElementDescription;
};