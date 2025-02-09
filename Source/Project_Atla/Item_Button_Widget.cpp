// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Button_Widget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Ren_Low_Poly_Character.h"
#include "Inventory_List_Widget.h"
#include "Kismet/GameplayStatics.h"



void UItem_Button_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    if (!Item_Button)
    {
        UE_LOG(LogTemp, Warning, TEXT("Item_Button is nullptr!"));
        return;
    }

    // Clear existing delegates before adding new ones
    Item_Button->OnClicked.Clear();
    Item_Button->OnHovered.Clear();
    Item_Button->OnUnhovered.Clear();

    // Bind delegates
    Item_Button->OnClicked.AddDynamic(this, &UItem_Button_Widget::OnItemButtonClicked);
    Item_Button->OnHovered.AddDynamic(this, &UItem_Button_Widget::OnItemButtonHovered);
    Item_Button->OnUnhovered.AddDynamic(this, &UItem_Button_Widget::OnItemButtonUnhovered);
}




void UItem_Button_Widget::OnItemButtonClicked()
{


    if (PlayerCharacter && CurrentItem.Item)
    {
        if (UInventory* InventoryComp = PlayerCharacter->FindComponentByClass<UInventory>())
        {
            // Get the default object to access its properties
            if (ABase_Item* DefaultItem = CurrentItem.Item.GetDefaultObject())
            {
                PlayerCharacter->CurrentItemBeingUsed = CurrentItem;
                PlayerCharacter->SpawnActionBanner(DefaultItem->ItemName);  // Use the ItemName property
                ReturnToGameplay();
            }
        }
    }


}




void UItem_Button_Widget::OnItemButtonHovered()
{

    if (ParentListWidget && ParentListWidget->DescriptionText)
    {
        if (CurrentItem.Item)
        {
            ABase_Item* DefaultItem = CurrentItem.Item.GetDefaultObject();
            if (DefaultItem)
            {
                // You might want to add a Description property to your Base_Item class
                ParentListWidget->DescriptionText->SetText(FText::FromString(DefaultItem->Item_Description));
            }
        }
    }

}

void UItem_Button_Widget::OnItemButtonUnhovered()
{

    if (ParentListWidget && ParentListWidget->DescriptionText)
    {
        ParentListWidget->DescriptionText->SetText(FText::GetEmpty());
    }

}



void UItem_Button_Widget::SetupButton(FInventoryItem ItemData, ARen_Low_Poly_Character* Character)
{

    CurrentItem = ItemData;
    PlayerCharacter = Character;

    if (CurrentItem.Item)
    {
        ABase_Item* DefaultItem = CurrentItem.Item.GetDefaultObject();
        if (DefaultItem)
        {
            // Set the name
            if (Item_Name)
            {
                Item_Name->SetText(FText::FromString(DefaultItem->ItemName));
            }

            // Set the quantity
            if (Item_Quantity)
            {
                FString QuantityText = FString::Printf(TEXT("x%d"), CurrentItem.Quantity);
                Item_Quantity->SetText(FText::FromString(QuantityText));
            }

            // Store the description
            CurrentItem.ItemDescription = DefaultItem->Item_Description;
        }
    }

}




void UItem_Button_Widget::SetParentList(UInventory_List_Widget* InParentList)
{

    ParentListWidget = InParentList;



}




FReply UItem_Button_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Enter ||
        InKeyEvent.GetKey() == EKeys::SpaceBar ||
        InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
    {
        if (Item_Button)
        {
            OnItemButtonClicked();
            return FReply::Handled();
        }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}





FReply UItem_Button_Widget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    if (Item_Button)
    {
        if (ParentListWidget && ParentListWidget->DescriptionText)
        {
            if (CurrentItem.Item)
            {
                ABase_Item* DefaultItem = CurrentItem.Item.GetDefaultObject();
                if (DefaultItem)
                {
                    ParentListWidget->DescriptionText->SetText(FText::FromString(DefaultItem->ItemName));
                }
            }
        }
    }

    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}




void UItem_Button_Widget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{






}
