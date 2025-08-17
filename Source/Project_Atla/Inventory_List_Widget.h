// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory_List_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UInventory_List_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:

    virtual void NativeConstruct() override;


    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UScrollBox* Item_ScrollBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* DescriptionText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<class UItem_Button_Widget> ItemButtonClass;


    void SetupWidget(class ARen_Low_Poly_Character* Character);
    void PopulateItemList();
    void SetupInputMode();

private:

    UPROPERTY()
        class ARen_Low_Poly_Character* PlayerCharacter;



protected:

    bool IsControllerConnected() const;

};
