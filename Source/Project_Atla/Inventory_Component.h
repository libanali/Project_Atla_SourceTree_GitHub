// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory_Component.generated.h"


class UItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_ATLA_API UInventory_Component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory_Component();

    // Maximum number of item types the inventory can hold
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
        int32 MaxInventorySlots;

    // The actual inventory
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
        TArray<UItem*> InventoryItems;

    // Add item to inventory
    bool AddItem(UItem* Item);

    // Remove item if quantity is 0
    void RemoveItem(UItem* Item);

    // Check if inventory has space
    bool HasSpace() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
