// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Class.h"
#include "Strength_Potion.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UStrength_Potion : public UItem_Class
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
        float StrengthIncreasePercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
        float Duration;


    UStrength_Potion()
    {
        ItemName = "Strength Potion";
        StrengthIncreasePercentage = 2.2f;
        Duration = 3.0f; // Example duration

    }

    virtual void UseItem(class ARen_Low_Poly_Character* Ren) override;
	
};
