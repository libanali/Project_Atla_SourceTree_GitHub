// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Health_Potion.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UHealth_Potion : public UItem
{
	GENERATED_BODY()


public:
    UHealth_Potion();

    virtual void Use(class ARen_Low_Poly_Character* Ren) override;
};
