// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item_Button_Widget.h"
#include "Inventory_Widget.generated.h"

/**
 * 
 */


UCLASS()
class PROJECT_ATLA_API UInventory_Widget : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UScrollBox* ItemScrollBox;


	UPROPERTY(meta = (BindWidget))
		class UBorder* Border;


	UFUNCTION(BlueprintCallable)
		void AddItemToScrollBox(UItem_Class* Item);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> ItemButtonWidgetClass;

	
};
