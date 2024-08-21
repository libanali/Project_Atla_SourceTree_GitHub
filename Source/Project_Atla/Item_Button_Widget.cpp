// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Button_Widget.h"

void UItem_Button_Widget::NativeConstruct()
{

	Super::NativeConstruct();

}

void UItem_Button_Widget::SetItemDetails(const FName& TheItemName, int32 Quantity)
{

    if (ItemText)
    {
        ItemText->SetText(FText::FromString(FString::Printf(TEXT("%s x%d"), *TheItemName.ToString(), Quantity)));
    }

}

void UItem_Button_Widget::UpdateQuantity(int32 NewQuantity)
{

    CurrentQuantity = NewQuantity;

    if (ItemText)
    {
        ItemText->SetText(FText::FromString(FString::Printf(TEXT("%s x%d"), *ItemName.ToString(), CurrentQuantity)));
    }

}
