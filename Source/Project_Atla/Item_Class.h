// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item_Class.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECT_ATLA_API UItem_Class : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
        FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
        int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
        int32 CurrentStackSize;


    UItem_Class()
    {
        ItemName = "Default";
        MaxStackSize = 1;
        CurrentStackSize = 1;
    }

    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void UseItem(class ARen_Low_Poly_Character* Ren);
	
};
