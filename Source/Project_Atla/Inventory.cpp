// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxInventorySize = 20;


	// ...
}


bool UInventory::AddItem(TSubclassOf<ABase_Item> ItemToAdd)
{
    // Get the default object to check its properties
    if (!ItemToAdd)
        return false;

    // Get the default object to check its properties
    ABase_Item* ItemDefault = ItemToAdd.GetDefaultObject();

    // If item is stackable, try to find existing stack
    if (ItemDefault->bIsStackable)
    {
        int32 ExistingIndex = FindExistingItem(ItemToAdd);
        if (ExistingIndex != -1)
        {
            // Found existing stack, check if we can add to it
            ABase_Item* ExistingItem = Inventory[ExistingIndex].Item.GetDefaultObject();
            if (Inventory[ExistingIndex].Quantity < ExistingItem->MaxStackSize)
            {
                // Add to existing stack
                Inventory[ExistingIndex].Quantity++;
                return true;
            }
        }
    }

    // If we couldn't stack (or item isn't stackable), try to add to new slot
    if (Inventory.Num() < MaxInventorySize)
    {
        FInventoryItem NewItem;
        NewItem.Item = ItemToAdd;
        NewItem.Quantity = 1;
        Inventory.Add(NewItem);
        return true;
    }

    return false;
}





int32 UInventory::FindExistingItem(TSubclassOf<ABase_Item> ItemToAdd)
{
    // Get the default object to check its properties
    ABase_Item* ItemDefault = ItemToAdd.GetDefaultObject();
    FString ItemNameToFind = ItemDefault->ItemName;

    // Look through inventory for matching item
    for (int32 i = 0; i < Inventory.Num(); i++)
    {
        ABase_Item* ExistingItem = Inventory[i].Item.GetDefaultObject();
        if (ExistingItem && ExistingItem->ItemName == ItemNameToFind)
        {
            return i;
        }
    }
    return -1;
}



// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}





