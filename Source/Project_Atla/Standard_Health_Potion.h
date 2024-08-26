// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Class.h"
#include "Standard_Health_Potion.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UStandard_Health_Potion : public UItem_Class
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
		float HealthIncreasePercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
		float Duration;


	UStandard_Health_Potion()
	{

		ItemName = "Standard Health Potion";
		HealthIncreasePercentage = 2.2f;
	}


	virtual void UseItem(class ARen_Low_Poly_Character* Ren) override;

	
};
