// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Token_Manager.h"

// Sets default values
AEnemy_Token_Manager::AEnemy_Token_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxTokens = 3;

}

bool AEnemy_Token_Manager::GrantToken(AEnemy_Poly* Enemy)
{

	if (ActiveEnemies.Num() < MaxTokens && !ActiveEnemies.Contains(Enemy))

	{

		ActiveEnemies.Add(Enemy);
		return true;

	}


	return false;
}



void AEnemy_Token_Manager::RevokeToken(AEnemy_Poly* Enemy)
{

	ActiveEnemies.Remove(Enemy);

}



bool AEnemy_Token_Manager::HasToken(AEnemy_Poly* Enemy) const
{
	return ActiveEnemies.Contains(Enemy);
}



void AEnemy_Token_Manager::SetMaxTokens(int32 NewMaxTokens)
{

	MaxTokens = NewMaxTokens;
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

