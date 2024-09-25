// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Token_Manager.h"

// Sets default values
AEnemy_Token_Manager::AEnemy_Token_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy_Token_Manager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy_Token_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

