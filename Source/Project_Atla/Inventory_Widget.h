// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory_Widget.generated.h"

/**
 * 
 */

class UScrollBox;
class UItemButtonWidget;
class UItem_Class;

UCLASS()
class PROJECT_ATLA_API UInventory_Widget : public UUserWidget
{
	GENERATED_BODY()


public:
    UPROPERTY(meta = (BindWidget))
        UScrollBox* ItemScrollBox;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
        void AddItemToScrollBox(UItem_Class* Item);

protected:
    virtual void NativeConstruct() override;
	
};
