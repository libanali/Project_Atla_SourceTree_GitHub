// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "Ren_Low_Poly_Character.h"
// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxInventorySize = 20;

    bIsInventoryEmpty = true;
	// ...
}


bool UInventory::AddItem(TSubclassOf<ABase_Item> ItemToAdd)
{
    if (!ItemToAdd)
        return false;

    // Get default object to check properties
    ABase_Item* DefaultItem = ItemToAdd.GetDefaultObject();

    // Look for existing item in inventory
    for (FInventoryItem& InvItem : Inventory)
    {
        if (InvItem.Item == ItemToAdd)
        {
            if (DefaultItem->bIsStackable)
            {
                // Increase quantity if stackable
                InvItem.Quantity++;
                CheckCurrentInventory();
                OnInventoryUpdated.Broadcast();
                return true;
            }
            break;
        }
    }

    // If we didn't find it or it's not stackable, add new entry
    if (Inventory.Num() < MaxInventorySize)
    {
        FInventoryItem NewItem;
        NewItem.Item = ItemToAdd;
        NewItem.Quantity = 1;
        Inventory.Add(NewItem);
        OnInventoryUpdated.Broadcast();
        CheckCurrentInventory();
        return true;
    }

    return false;
}


void UInventory::UseItem(TSubclassOf<ABase_Item> ItemClass)
{

    for (int32 i = 0; i < Inventory.Num(); i++)
    {
        if (Inventory[i].Item == ItemClass)
        {
            // Spawn temporary item to use its effect
            if (ABase_Item* Item = GetWorld()->SpawnActor<ABase_Item>(ItemClass))
            {
                Item->UseItem(GetOwner());
                Item->Destroy();
            }

            // Decrease quantity
            Inventory[i].Quantity--;

            // Remove item if quantity is 0
            if (Inventory[i].Quantity <= 0)
            {
                Inventory.RemoveAt(i);
            }

            OnInventoryUpdated.Broadcast();
            CheckCurrentInventory();
            break;
        }
    }
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




void UInventory::CheckCurrentInventory()
{
    bool bWasEmpty = bIsInventoryEmpty;
    bIsInventoryEmpty = (Inventory.Num() == 0);

    // If the inventory state changed, update UI
    if (bWasEmpty != bIsInventoryEmpty)
    {
        if (ARen_Low_Poly_Character* Character = Cast<ARen_Low_Poly_Character>(GetOwner()))
        {
            if (Character->CommandMenuWidget)
            {
                Character->CommandMenuWidget->CheckInventoryAndSetFocus();
            }
        }
    }
}



// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}





