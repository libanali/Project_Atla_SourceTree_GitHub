// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_Widget.h"
#include "Components/ScrollBox.h"
#include "Item_Button_Widget.h"
#include "Item_Class.h"
#include "Ren_Low_Poly_Character.h"

void UInventory_Widget::NativeConstruct()
{

	Super::NativeConstruct();

   
}


void UInventory_Widget::AddItemToScrollBox(UItem_Class* Item)
{

    if (!ItemScrollBox || !ItemButtonWidgetClass || !Item)
    {

        UE_LOG(LogTemp, Warning, TEXT("ItemScrollBox, ItemButtonWidgetClass, or Item is null."));
        return;

    }

    bool bItemFound = false;

    // Iterate over all children in the ScrollBox to see if the item already exists
    for (UWidget* Widget : ItemScrollBox->GetAllChildren())
    {
        UItem_Button_Widget* ItemButton = Cast<UItem_Button_Widget>(Widget);
        if (ItemButton && ItemButton->ItemName == Item->ItemName)
        {
            // Item already exists, update the quantity
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, TEXT("Added!!"));
            ItemButton->UpdateQuantity(ItemButton->CurrentQuantity + Item->CurrentStackSize);
            bItemFound = true;
            break;
        }
    }



    // If item not found, create a new button
    if (!bItemFound)
    {

        UItem_Button_Widget* NewItemButton = CreateWidget<UItem_Button_Widget>(this, UItem_Button_Widget::StaticClass());

        if (NewItemButton)
        {
            NewItemButton->SetItemDetails(Item->ItemName, Item->CurrentStackSize);
            ItemScrollBox->AddChild(NewItemButton);
            UE_LOG(LogTemp, Log, TEXT("Widget created for %s"), *Item->ItemName.ToString());
        }

        else

        {

            UE_LOG(LogTemp, Warning, TEXT("Failed to create widget for %s"), *Item->ItemName.ToString());

        }
    }

}

