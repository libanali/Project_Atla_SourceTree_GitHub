// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Class.h"
#include "WorldItem.generated.h"

UCLASS()
class PROJECT_ATLA_API AWorldItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UItem_Class* Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FName ItemName;

	UFUNCTION(BlueprintCallable, Category = "Item")
		void InitializeItem(UItem_Class* NewItem);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		TSubclassOf<class UItem_Class> ItemClass;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
		UItem_Class* ItemInstance;


	UFUNCTION(BlueprintCallable, Category = "Item")
		void UseItem(class ARen_Low_Poly_Character* Ren);



};
