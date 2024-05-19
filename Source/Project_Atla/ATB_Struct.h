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


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float CurrentATB;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float MaxATB;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float ATBFillRate;

	float ATB_Attack_Boost;

	//	void ATB_Increase();

	void InitialiseATB();

	//ARen_Character* RenCharacter;


	FATB_Struct();
};
