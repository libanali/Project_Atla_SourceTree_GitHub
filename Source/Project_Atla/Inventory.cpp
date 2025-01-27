// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "Ren_Low_Poly_Character.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxInventorySize = 10;

    bIsInventoryEmpty = true;


}


bool UInventory::AddItem(TSubclassOf<ABase_Item> ItemToAdd)
{

    //GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Yellow,
        //FString::Printf(TEXT("Current Inventory Size: %d/%d"), Inventory.Num(), MaxInventorySize));

    if (!ItemToAdd)
        return false;

    // First verify NotificationWidget exists
    if (!NotificationWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("NotificationWidget is null"));
    }

    int32 totalItems = 0;
    for (const FInventoryItem& InvItem : Inventory)
    {
        totalItems += InvItem.Quantity;
    }

    // Get item name for notifications
    FString ItemName = TEXT("Unknown Item");
    if (ABase_Item* ItemDefault = ItemToAdd.GetDefaultObject())
    {
        ItemName = ItemDefault->ItemName;  // Assuming your Base_Item has an ItemName property
    }

    if (totalItems >= MaxInventorySize)
    {
        // Inventory full notification
        if (NotificationWidget && IsValid(NotificationWidget))
        {
            NotificationWidget->AddNotification(TEXT("Inventory Full!"), 3.0f);
        }
        return false;
    }

    // Check for existing item
    for (FInventoryItem& InvItem : Inventory)
    {
        if (InvItem.Item == ItemToAdd)
        {
            InvItem.Quantity++;

            // Show stacked item notification
            if (NotificationWidget && IsValid(NotificationWidget))
            {
                FString Message = FString::Printf(TEXT("%s x1"), *ItemName);
                NotificationWidget->AddNotification(Message, 3.0f);
            }

            OnInventoryUpdated.Broadcast();
            CheckCurrentInventory();
            return true;
        }
    }

    // If item not found, create new slot
    FInventoryItem NewItem;
    NewItem.Item = ItemToAdd;
    NewItem.Quantity = 1;
    Inventory.Add(NewItem);

    // Show new item notification
    if (NotificationWidget && IsValid(NotificationWidget))
    {
        FString Message = FString::Printf(TEXT("%s x1"), *ItemName);
        NotificationWidget->AddNotification(Message, 3.0f);
    }

    OnInventoryUpdated.Broadcast();
    CheckCurrentInventory();
    return true;
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

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
        FString::Printf(TEXT("MaxInventorySize initialized to: %d"), MaxInventorySize));
	
}





