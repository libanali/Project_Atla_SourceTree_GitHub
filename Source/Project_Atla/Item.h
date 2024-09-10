// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Ren_Low_Poly_Character.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECT_ATLA_API UItem : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 Quantity;

	UItem();

	virtual void Use(ARen_Low_Poly_Character* Ren);


	
};
