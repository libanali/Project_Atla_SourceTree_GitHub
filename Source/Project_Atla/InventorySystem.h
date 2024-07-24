// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventorySystem.generated.h"





USTRUCT(BlueprintType)
struct FInventoryItem
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		AItem* Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 Quantity;

	FInventoryItem()
		: Item(nullptr), Quantity(0) {}

	FInventoryItem(AItem* InItem, int32 InQuantity)
		: Item(InItem), Quantity(InQuantity) {}

};



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_ATLA_API UInventorySystem : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UInventorySystem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		TArray<FInventoryItem> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 MaxSlots;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddItem(AItem* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool RemoveItem(AItem* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void UseItem(AItem* Item);

};
