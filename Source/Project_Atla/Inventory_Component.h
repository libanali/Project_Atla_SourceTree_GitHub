// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item_Class.h"
#include "Inventory_Component.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_ATLA_API UInventory_Component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory_Component();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<UItem_Class*> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 MaxInventorySlots;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddItem(UItem_Class* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool RemoveItem(UItem_Class* Item);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool ItemAdded;

	// Function to return the items in the inventory
	const TArray<UItem_Class*>& GetItems() const;



		
};
