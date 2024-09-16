// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Pick_Up_Actor.generated.h"

class UItem;
class UBoxComponent; 
class UStaticMeshComponent;

UCLASS()
class PROJECT_ATLA_API AItem_Pick_Up_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem_Pick_Up_Actor();

	UPROPERTY(EditAnywhere, Category = "Item")
		UItem* Item;

	UItem* GetItem() const { return Item; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

};
