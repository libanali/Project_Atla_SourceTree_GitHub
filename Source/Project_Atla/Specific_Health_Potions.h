// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Health_Potion.h"
#include "Specific_Health_Potions.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UHealthPotion30 : public UHealth_Potion
{
	GENERATED_BODY()

public:
    UHealthPotion30()
    {
        ItemName = "Health Potion";
        HealthIncreasePercentage = 0.2f;
    }
};

UCLASS()
class PROJECT_ATLA_API USuperHealthPotion : public UHealth_Potion
{
    GENERATED_BODY()

public:
    USuperHealthPotion()
    {
        ItemName = "Super Health Potion";
        HealthIncreasePercentage = 0.35f;
    }
};

UCLASS()
class PROJECT_ATLA_API UUltimateHealthPotion : public UHealth_Potion
{
    GENERATED_BODY()

public:
    UUltimateHealthPotion()
    {
        ItemName = "Ultimate Health Potion";
        HealthIncreasePercentage = 0.5f;
    }
};

	
