// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mana_Struct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FMana_Struct
{
    GENERATED_BODY()

public:


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana")
    float MaxMana = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
        float CurrentMana = 100.0f;



    FMana_Struct();

    void InitializeMana();


    void IncreaseMana(float ManaAmount);

    void DecreaseMana(float DecreaseAmount);



};

