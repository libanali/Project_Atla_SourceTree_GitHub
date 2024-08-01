// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Class.h"
#include "Health_Potion.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UHealth_Potion : public UItem_Class
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
        float HealthIncreasePercentage;

    UHealth_Potion()
    {
        HealthIncreasePercentage = 0.0f;
    }

    virtual void Use(class ARen_Low_Poly_Character* Ren) override;

	
};
