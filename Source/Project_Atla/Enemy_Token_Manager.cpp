// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Token_Manager.h"
#include "Enemy_AIController.h"

// Sets default values
AEnemy_Token_Manager::AEnemy_Token_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

void AEnemy_Token_Manager::RegisterEnemy(AEnemy_AIController* EnemyController)
{

	EnemyControllers.Add(EnemyController);

}


void AEnemy_Token_Manager::EnemyTurn(AEnemy_AIController* EnemyController)
{

    if (EnemyController)
    {
        EnemyController->UpdateState();// Call update to handle AI behavior
    }

}

void AEnemy_Token_Manager::NextTurn()
{

    if (EnemyControllers.Num() > 0)
    {
        EnemyControllers[CurrentTurnIndex]->bIsAttacking = false; // Reset attacking flag
        CurrentTurnIndex = (CurrentTurnIndex + 1) % EnemyControllers.Num(); // Cycle through
        EnemyTurn(EnemyControllers[CurrentTurnIndex]);
    }

}


