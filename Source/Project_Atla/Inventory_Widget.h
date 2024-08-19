// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Inventory_Widget.generated.h"

/**
 * 
 */

class UScrollBox;
class UBorder;
class UItemButtonWidget;
//class UVerticalBox;
class UItem_Class; 

UCLASS()
class PROJECT_ATLA_API UInventory_Widget : public UUserWidget
{
	GENERATED_BODY()


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        UScrollBox* ItemScrollBox;


    UFUNCTION(BlueprintCallable, Category = "Inventory")
        void AddItemToScrollBox(UItem_Class* Item);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool AddedToScrollBox;

protected:
    virtual void NativeConstruct() override;
	
};
