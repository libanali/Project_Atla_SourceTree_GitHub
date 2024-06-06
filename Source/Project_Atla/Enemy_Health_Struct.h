// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy_Health_Struct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECT_ATLA_API FEnemy_Health_Struct
{

	GENERATED_BODY()

public:

	FEnemy_Health_Struct();


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
		float MaxHealth;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth;


	void InitializeHealth();


};
