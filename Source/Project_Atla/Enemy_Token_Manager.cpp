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
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
            FString::Printf(TEXT("Enemy %s Registered!"), *EnemyController->GetName()));
    }

}

void AEnemy_Token_Manager::NextTurn()
{
    if (EnemyControllers.IsValidIndex(CurrentEnemyIndex))
    {
        // Revoke token from previous enemy
        EnemyControllers[CurrentEnemyIndex]->HasToken = false;
    }

    // Advance to the next enemy
    CurrentEnemyIndex = (CurrentEnemyIndex + 1) % EnemyControllers.Num();

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Next turn, current enemy index: %d"), CurrentEnemyIndex));

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

void AEnemy_Token_Manager::GiveToken(AEnemy_AIController* EnemyController)
{

    if (EnemyController)
    {
        EnemyController->HasToken = true; // Set HasToken to true for this enemy
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Token given to enemy."));
    }
}



void AEnemy_Token_Manager::EndTurn(AEnemy_AIController* EnemyController)
{

    if (EnemyController)
    {
        EnemyController->HasToken = false; // Revoke the token
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Token revoked from enemy."));
    }

}



void AEnemy_Token_Manager::HandleNextTurn()
{
    if (EnemyControllers.Num() > 0)
    {
        if (CurrentEnemyIndex >= 0 && CurrentEnemyIndex < EnemyControllers.Num())
        {
            AEnemy_AIController* CurrentEnemy = EnemyControllers[CurrentEnemyIndex];
            if (CurrentEnemy)
            {
                // Ensure token is given to the current enemy
                CurrentEnemy->HasToken = true;
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                    FString::Printf(TEXT("Enemy %s received token!"), *CurrentEnemy->GetName()));
            }
        }

        // Set timer for next turn
        GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, this, &AEnemy_Token_Manager::NextTurn, TurnDuration, false);
    }
}


void AEnemy_Token_Manager::BeginPlay()
{
    Super::BeginPlay();

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Token Manager Initialized!"));

    StartTokenSystem();

}

void AEnemy_Token_Manager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
