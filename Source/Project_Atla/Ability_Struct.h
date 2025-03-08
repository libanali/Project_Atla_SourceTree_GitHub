// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability_Struct.generated.h"

/**
 * 
 */

class ARen_Low_Poly_Character;

USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FAbility_Struct
{
public:

	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
		float MaxAbilityPoints = 150.0f;


	UPROPERTY(BlueprintReadWrite, Category = "Ability")
		float CurrentAbilityPoints = 0.0f;


	FAbility_Struct();

	void InitializeAbilityPoints();


};
