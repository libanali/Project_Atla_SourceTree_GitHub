// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item_Button_Widget.generated.h"

/**
 * 
 */


class UButton;
class UTextBlock;


UCLASS()
class PROJECT_ATLA_API UItem_Button_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
        UButton* ItemButton;

    UPROPERTY(meta = (BindWidget))
        UTextBlock* ItemText;

    UFUNCTION(BlueprintCallable, Category = "Item")
        void SetItemDetails(const FName& ItemName, int32 Quantity);

protected:
    virtual void NativeConstruct() override;
	
};
