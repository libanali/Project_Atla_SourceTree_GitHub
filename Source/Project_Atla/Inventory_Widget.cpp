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


        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Added"));
    
        AddedToScrollBox = true;
        
        UE_LOG(LogTemp, Log, TEXT("Button added to scroll box."));


}
    else
    {
        // Log failure to create widget
        UE_LOG(LogTemp, Warning, TEXT("Failed to create ItemButton widget."));
    }
    

}

void UInventory_Widget::NativeConstruct()
{

    Super::NativeConstruct();

}
