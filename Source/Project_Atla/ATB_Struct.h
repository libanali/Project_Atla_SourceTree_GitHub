// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ATB_Struct.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FATB_Struct
{

	GENERATED_BODY()


public:

	FATB_Struct();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float CurrentATB = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float MaxATB = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float ATBFillRate = 1.2f;

	float ATB_Attack_Boost = 10.0f;

	//	void ATB_Increase();

	void InitialiseATB();

	//ARen_Character* RenCharacter;



};
