// Fill out your copyright notice in the Description page of Project Settings.


#include "Treasure_Chest.h"

// Sets default values
ATreasure_Chest::ATreasure_Chest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the box collider
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	RootComponent = BoxCollider;

	// Set up the box collider properties
	BoxCollider->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f)); // Adjust size as needed
	BoxCollider->SetCollisionProfileName(TEXT("Trigger")); // Set collision profile to trigger
	
}



void ATreasure_Chest::HandleChestReward(ARen_Low_Poly_Character* Ren)
{

	switch (ChestType)

	{

	case ETreasureChestType::AttackBoost:
		Ren->IncreaseAttack(1.6);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Increase Attack"));
		break;

	case ETreasureChestType::DefenseBoost:
		Ren->IncreaseDefence(1.7);
		break;

	case ETreasureChestType::UnlockTechnique:
		Ren->UnlockTechnique(TEXT("Fury Strike"));
		break;

	case ETreasureChestType::RareItem:
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("yep"));


	default:
		break;
	}



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

