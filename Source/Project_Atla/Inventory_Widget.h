// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item_Button_Widget.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Inventory_Widget.generated.h"

/**
 * 
 */

class UScrollBox;

class UInventory_Component; // Forward declaration


UCLASS()
class PROJECT_ATLA_API UInventory_Widget : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		 UScrollBox* ItemScrollBox;

	UFUNCTION(BlueprintCallable)
		void AddItemToScrollBox(UItem_Class* Item);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> ItemButtonWidgetClass;


private:

	UInventory_Component* InventoryComponent;


	
};
