// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_Component.h"
#include "Item.h"

// Sets default values for this component's properties
UInventory_Component::UInventory_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxInventorySlots = 10;

	// ...
}


bool UInventory_Component::AddItem(UItem* Item)
{
    if (!Item)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AddItem failed: Item is null"));
        return false;  // Return false if the item is null
    }

    // Debug: Show which item we're trying to add
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Trying to add item: %s"), *Item->ItemName.ToString()));

    int32 ItemIndex = FindItemIndex(Item);

    // Debug: Show if we found an item index
    if (ItemIndex == INDEX_NONE)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Item not found in inventory, adding as new item"));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Item found at index: %d"), ItemIndex));
    }

    if (ItemIndex != INDEX_NONE)
    {
        UItem* ExistingItem = InventoryItems[ItemIndex];
        int32 RemainingSpace = ExistingItem->MaxStackSize - ExistingItem->Quantity;

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Remaining space: %d"), RemainingSpace));

        if (RemainingSpace > 0)
        {
            int32 AmountToAdd = FMath::Min(RemainingSpace, Item->Quantity);
            ExistingItem->Quantity += AmountToAdd;
            Item->Quantity -= AmountToAdd;

            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Added %d items to existing stack"), AmountToAdd));

            if (Item->Quantity > 0 && HasSpace())
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Stacking remaining items..."));
                AddItem(Item);  // Recursively add the remaining items
            }

            return true;
        }
    }

    if (HasSpace())
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Adding new item to inventory"));
        InventoryItems.Add(Item);
        return true;  // Return true after adding a new item
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Inventory full or item couldn't be added!"));
    return false;
}

int32 UInventory_Component::FindItemIndex(UItem* Item) const
{
    for (int32 i = 0; i < InventoryItems.Num(); i++)
    {
        if (InventoryItems[i]->ItemName == Item->ItemName)
        {
            return i;  // Return index if found
        }
    }
    return INDEX_NONE;
}

void UInventory_Component::RemoveItem(UItem* Item)
{

    if (Item->Quantity <= 0)

    {

        InventoryItems.Remove(Item);

    }


}

bool UInventory_Component::HasSpace() const
{
    return InventoryItems.Num() < MaxInventorySlots;

}

// Called when the game starts
void UInventory_Component::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventory_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

