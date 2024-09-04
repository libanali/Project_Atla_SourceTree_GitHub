// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Button_Widget.h"
#include "Item_Class.h"
#include "Ren_Low_Poly_Character.h"


void UItem_Button_Widget::NativeConstruct()
{

	Super::NativeConstruct();

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("BUTTON!!!!!!!!!!!!!!!!!!!!!!"));

}

void UItem_Button_Widget::SetItemDetails(const FName& TheItemName, int32 Quantity)
{

    if (ItemText)
    {
        ItemText->SetText(FText::FromString(FString::Printf(TEXT("%s"), *TheItemName.ToString())));
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
