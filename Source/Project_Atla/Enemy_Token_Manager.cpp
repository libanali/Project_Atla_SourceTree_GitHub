// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Token_Manager.h"
#include "Enemy_AIController.h"
#include "Kismet/GameplayStatics.h"


AEnemy_Token_Manager::AEnemy_Token_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    TurnDuration = 3.0f; // Example duration for each enemy's turn
    CurrentEnemyIndex = 0;


}

void AEnemy_Token_Manager::RegisterEnemy(AEnemy_AIController* EnemyController)
{

    if (EnemyController && !EnemyControllers.Contains(EnemyController))
    {
        EnemyControllers.Add(EnemyController);
    }

}

void AEnemy_Token_Manager::NextTurn()
{

    // Revoke token from the previous enemy
    if (EnemyControllers.IsValidIndex(CurrentEnemyIndex))
    {
        EnemyControllers[CurrentEnemyIndex]->HasToken = false;
    }

    // Advance to the next enemy
    CurrentEnemyIndex = (CurrentEnemyIndex + 1) % EnemyControllers.Num();

    // Start the next turn
    HandleNextTurn();


}

void AEnemy_Token_Manager::StartTokenSystem()
{

    if (EnemyControllers.Num() > 0)
    {
        // Start the token system by initiating the first turn
        HandleNextTurn();
    }


}



void AEnemy_Token_Manager::HandleNextTurn()
{

    if (EnemyControllers.Num() > 0 && EnemyControllers.IsValidIndex(CurrentEnemyIndex))
    {
        // Assign token to the current enemy to allow it to attack
        AEnemy_AIController* CurrentEnemy = EnemyControllers[CurrentEnemyIndex];
        if (CurrentEnemy)
        {
            CurrentEnemy->HasToken = true;
            CurrentEnemy->AttackPlayer();
        }

        // Tell all other enemies to strafe
        for (int32 i = 0; i < EnemyControllers.Num(); i++)
        {
            if (i != CurrentEnemyIndex)
            {
                AEnemy_AIController* OtherEnemy = EnemyControllers[i];
                if (OtherEnemy)
                {
                    OtherEnemy->HasToken = false;
                    OtherEnemy->StrafeAroundPlayer();
                }
            }
        }

        // Move to the next enemy after the set duration
        GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, this, &AEnemy_Token_Manager::NextTurn, TurnDuration, false);
    }


}

void AEnemy_Token_Manager::BeginPlay()
{
    Super::BeginPlay();

    StartTokenSystem();

}

void AEnemy_Token_Manager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
