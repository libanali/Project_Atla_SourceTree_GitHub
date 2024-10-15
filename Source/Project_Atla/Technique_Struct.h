// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Technique_Struct.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FTechnique_Struct
{
    GENERATED_BODY()

public:

    // Default constructor
    FTechnique_Struct()
        : TechniqueName(TEXT("Unknown")), Description(TEXT("No Description")), bIsUnlocked(false), TechniqueAnimation(nullptr), DamageBonus(1.0f), PointsRequired(1) {}

    // Parameterized constructor
    FTechnique_Struct(FString Name, FString Desc, bool bUnlocked, UAnimMontage* AnimMontage, float InDamageBonus, int32 InPointsRequired)
        : TechniqueName(Name), Description(Desc), bIsUnlocked(bUnlocked), TechniqueAnimation(AnimMontage), DamageBonus(InDamageBonus), PointsRequired(InPointsRequired) {}


    // Current value of the technique gauge
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique System")
        float CurrentGauge;

    // Maximum value of the technique gauge (when reached, a skill point is awarded)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique System")
        float MaxGauge;

    // Total technique points earned by the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique System")
        int32 TechniquePoints;

    // Name of the technique
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
        FString TechniqueName;

    // Description of the technique
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
        FString Description;

    // Boolean to check if the technique is unlocked
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
        bool bIsUnlocked;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique Animation")
        UAnimMontage* TechniqueAnimation;

    // Damage bonus when using a technique
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique System")
        float DamageBonus;

    // Points required to use the technique
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique System")
        int32 PointsRequired;
	
};
