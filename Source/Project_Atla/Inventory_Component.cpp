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
    for (UItem* ExistingItem : InventoryItems)
    {
        if (ExistingItem->ItemName == Item->ItemName)
        {
            if (ExistingItem->Quantity + Item->Quantity <= ExistingItem->MaxStackSize)
            {
                ExistingItem->Quantity += Item->Quantity;
                return true;
            }
        }
    }

    // Add new item if space is available
    if (HasSpace())
    {
        InventoryItems.Add(Item);
        return true;
    }

    // Inventory is full
    return false;
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

