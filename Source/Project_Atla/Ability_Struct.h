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

	FAbility_Struct();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
		float MaxAbilityPoints;


	UPROPERTY(BlueprintReadWrite, Category = "Ability")
		float CurrentAbilityPoints;


	void InitializeAbilityPoints();

	void IncreaseAbilityPoints(float AbilityPointsAmount);





};
