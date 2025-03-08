// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Health_Struct.generated.h"

/**
 * 
 */

class ARen_Low_Poly_Character;

USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FHealth_Struct
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
        float MaxHealth = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Health")
        float CurrentHealth = 100.0f;


    FHealth_Struct();


    void InitializeHealth();


    void TakeDamage(float DamageAmount);


    void IncreaseHealth(float HealAmount);


 

};
