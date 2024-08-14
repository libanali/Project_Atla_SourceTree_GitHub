// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Button_Widget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UItem_Button_Widget::SetItemDetails(const FName& ItemName, int32 Quantity)
{

    if (ItemText)
    {
        ItemText->SetText(FText::FromString(FString::Printf(TEXT("%s x%d"), *ItemName.ToString(), Quantity)));
    }
}

void UItem_Button_Widget::NativeConstruct()
{

    Super::NativeConstruct();


}
