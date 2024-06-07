// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Low_Poly_Character.h"

// Sets default values
ARen_Low_Poly_Character::ARen_Low_Poly_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARen_Low_Poly_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARen_Low_Poly_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARen_Low_Poly_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

