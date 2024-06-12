// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Poly.h"

// Sets default values
AEnemy_Poly::AEnemy_Poly()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Attacking = false;


}

bool AEnemy_Poly::EnemyIsDead() const
{
	return false;
}

void AEnemy_Poly::Death()
{
}

void AEnemy_Poly::InflictDamageOnCharacter(ARen_Character* RenCharacter)
{
}

// Called when the game starts or when spawned
void AEnemy_Poly::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy_Poly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy_Poly::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

