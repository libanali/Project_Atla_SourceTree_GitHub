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
    if (!Item) return false;  // Return false if the item is null

    int32 ItemIndex = FindItemIndex(Item);

    if (ItemIndex != INDEX_NONE)
    {
        UItem* ExistingItem = InventoryItems[ItemIndex];
        int32 RemainingSpace = ExistingItem->MaxStackSize - ExistingItem->Quantity;

        if (RemainingSpace > 0)
        {
            int32 AmountToAdd = FMath::Min(RemainingSpace, Item->Quantity);
            ExistingItem->Quantity += AmountToAdd;
            Item->Quantity -= AmountToAdd;

            if (Item->Quantity > 0)
            {
                AddItem(Item);  // Recursively add the remaining items
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Stacked")));

            }

            return true;  // Return true after successfully stacking the item
        }
    }

    if (InventoryItems.Num() < MaxInventorySlots)
    {
        InventoryItems.Add(Item);
        return true;  // Return true after adding a new item
    }

    return false;  // Return false if the item couldn't be added
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

