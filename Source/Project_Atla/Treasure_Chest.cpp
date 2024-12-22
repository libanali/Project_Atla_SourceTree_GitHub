// Fill out your copyright notice in the Description page of Project Settings.


#include "Treasure_Chest.h"



// Sets default values
ATreasure_Chest::ATreasure_Chest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsOpen = false;

	AttackIncrease = 3.0f;
	DefenceIncrease = 2.0f;

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

// Called to bind functionality to input
void ATreasure_Chest::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATreasure_Chest::HandleChestReward(ARen_Low_Poly_Character* Ren)
{

	switch(ChestType)
	{

	case ETreasureChestType::AddPoints:
		//Ren->IncreaseAttack(Ren->AttackIncrease);
		break;

	case ETreasureChestType::RareItems:
		//Ren->UnlockTechnique(TechniqueName);
		break;

	default:
		break;


	}


}


