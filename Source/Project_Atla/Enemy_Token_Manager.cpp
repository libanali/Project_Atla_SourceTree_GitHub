// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Token_Manager.h"
#include "Enemy_AIController.h"
#include "Kismet/GameplayStatics.h"


AEnemy_Token_Manager::AEnemy_Token_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    TurnDuration = 2.0f; // Example duration for each enemy's turn
    CurrentEnemyIndex = 0;


}

void AEnemy_Token_Manager::RegisterEnemy(AEnemy_AIController* EnemyController)
{


}

void AEnemy_Token_Manager::NextTurn()
{
   

}

void AEnemy_Token_Manager::StartTokenSystem()
{

}


void AEnemy_Token_Manager::GiveToken(AEnemy_AIController* EnemyController)
{

   
}



void AEnemy_Token_Manager::EndTurn(AEnemy_AIController* EnemyController)
{
   

}



void AEnemy_Token_Manager::HandleNextTurn()
{
   
}


void AEnemy_Token_Manager::BeginPlay()
{
    Super::BeginPlay();

    

    

}

void AEnemy_Token_Manager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
