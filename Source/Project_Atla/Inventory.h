// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Base_Item.h"
#include "Inventory.generated.h"


USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<ABase_Item> Item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Quantity;

    FInventoryItem()
    {
        Item = nullptr;
        Quantity = 0;
    }
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_ATLA_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


    // Changed from TArray<TSubclassOf<ABase_Item>> to store quantities
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
        TArray<FInventoryItem> Inventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
        int32 MaxInventorySize;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
        bool AddItem(TSubclassOf<ABase_Item> ItemToAdd);

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
        FOnInventoryUpdated OnInventoryUpdated;

    // Function to use an item
    UFUNCTION(BlueprintCallable, Category = "Inventory")
        void UseItem(TSubclassOf<ABase_Item> ItemClass);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

    int32 FindExistingItem(TSubclassOf<ABase_Item> ItemToAdd);



		
};
