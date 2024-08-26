// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Item_Class.h"
#include "Item_Button_Widget.generated.h"

/**
 * 
 */


UCLASS()
class PROJECT_ATLA_API UItem_Button_Widget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct();

	UPROPERTY(meta = (BindWidget))
		UButton* ItemButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
		FName ItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
		UItem_Class* ItemInstance;

	UFUNCTION(BlueprintCallable)
		void SetItemDetails(const FName& TheItemName, int32 Quantity);

	UFUNCTION(BlueprintCallable)
		void UpdateQuantity(int32 NewQuantity);

	int32 CurrentQuantity;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget), Category = "Item Text")
		class UTextBlock* ItemText;

	
	
};
