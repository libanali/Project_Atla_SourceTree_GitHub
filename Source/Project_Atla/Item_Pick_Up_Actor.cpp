// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Pick_Up_Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Ren_Low_Poly_Character.h"
#include "Inventory_Component.h"
#include "Item.h"

// Sets default values
AItem_Pick_Up_Actor::AItem_Pick_Up_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

 
    

}

// Called when the game starts or when spawned
void AItem_Pick_Up_Actor::BeginPlay()
{
	Super::BeginPlay();

    
}

// Called every frame
void AItem_Pick_Up_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


