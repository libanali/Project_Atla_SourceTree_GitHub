// Fill out your copyright notice in the Description page of Project Settings.


#include "Treasure_Chest.h"

// Sets default values
ATreasure_Chest::ATreasure_Chest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATreasure_Chest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATreasure_Chest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

