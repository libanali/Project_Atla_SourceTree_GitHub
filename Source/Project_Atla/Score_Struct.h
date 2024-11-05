// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Score_Struct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FScore_Struct
{
	GENERATED_BODY()

public:

    // Constructor to initialize default values
    FScore_Struct()
        : BasePoints(0), RoundMultiplier(1.0f)
    {}

    // Base points for the enemy type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
        int32 BasePoints;

    // Multiplier for scaling points by round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
        float RoundMultiplier;



  
};
