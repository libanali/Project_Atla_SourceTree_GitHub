// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Class.h"
#include "Defence_Potion.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UDefence_Potion : public UItem_Class
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
        float DefenseIncreasePercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
        float Duration;

    UDefence_Potion()
    {
        ItemName = "Defense Potion";
        DefenseIncreasePercentage = 0.2f;
        Duration = 30.0f; // Example duration
    }

    virtual void UseItem(class ARen_Low_Poly_Character* Ren) override;
};
	
