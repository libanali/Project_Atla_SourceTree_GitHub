// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Class.h"
#include "Ultra_Health_Potion.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UUltra_Health_Potion : public UItem_Class
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
		float HealthIncreasePercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
		float Duration;


	UUltra_Health_Potion()
	{

		ItemName = "Strength Potion";
		HealthIncreasePercentage = 1.7f;
	}


	virtual void UseItem(class ARen_Low_Poly_Character* Ren) override;

	
};
