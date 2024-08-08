// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_Component.h"

// Sets default values for this component's properties
UInventory_Component::UInventory_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxInventorySlots = 10;
    ItemAdded = false;

	// ...
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

bool UInventory_Component::AddItem(UItem_Class* Item)
{
	
    if (!Item) return false;

    for (UItem_Class* ExistingItem : Items)
    {
        if (ExistingItem->ItemName == Item->ItemName && ExistingItem->CurrentStackSize < ExistingItem->MaxStackSize)
        {
            int32 StackSpace = ExistingItem->MaxStackSize - ExistingItem->CurrentStackSize;
            int32 AddAmount = FMath::Min(StackSpace, Item->CurrentStackSize);

            ExistingItem->CurrentStackSize += AddAmount;
            Item->CurrentStackSize -= AddAmount;

            if (Item->CurrentStackSize <= 0)
            {
                return true;
                ItemAdded = true;
            }
        }
    }

    if (Items.Num() < MaxInventorySlots)
    {
        Items.Add(Item);
        ItemAdded = true;
        return true;
    }

    return false;
}


bool UInventory_Component::RemoveItem(UItem_Class* Item)
{


    return Items.Remove(Item) > 0;



}

