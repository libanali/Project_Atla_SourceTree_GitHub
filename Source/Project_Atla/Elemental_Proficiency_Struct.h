// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elemental_Proficiency_Struct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)

struct PROJECT_ATLA_API FElemental_Proficiency_Struct
{

    GENERATED_BODY()

public:

 

    FElemental_Proficiency_Struct()
        : FireProficiency(0), IceProficiency(0), ThunderProficiency(0) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        float FireProficiency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        float IceProficiency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        float ThunderProficiency;

    // Element levels (starting from level 1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        int32 FireLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        int32 IceLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        int32 ThunderLevel = 1;


    // Thresholds for leveling up each elemental proficiency
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        TMap<int32, float> FireProficiencyThresholds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        TMap<int32, float> IceProficiencyThresholds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
        TMap<int32, float> ThunderProficiencyThresholds;

};
