// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_Widget.h"
#include "Components/ScrollBox.h"
#include "Item_Button_Widget.h" 
#include "Item_Class.h"

void UInventory_Widget::AddItemToScrollBox(UItem_Class* Item)
{

    if (!ItemScrollBox || !Item) return;

    // Create a new item button widget
    UItem_Button_Widget* ItemButton = CreateWidget<UItem_Button_Widget>(this, UItem_Button_Widget::StaticClass());
    if (ItemButton)
    {
        ItemButton->SetItemDetails(Item->ItemName, Item->CurrentStackSize);

        // Add the button to the scroll box
        ItemScrollBox->AddChild(ItemButton);
    }

    

}

void UInventory_Widget::NativeConstruct()
{

    Super::NativeConstruct();


}
