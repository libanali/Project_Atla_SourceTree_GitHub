// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "Engine/DataTable.h"
#include "Character_Attributes.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FCharacter_Attributes : public FTableRowBase
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
		int32 CharacterLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
		float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
		float BaseAttack = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
		float BaseDefence = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
		float BaseElemental = 100.0f;

		FCharacter_Attributes();


};
