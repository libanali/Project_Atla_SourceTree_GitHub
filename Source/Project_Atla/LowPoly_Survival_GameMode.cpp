// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_Survival_GameMode.h"
#include "Enemy.h"
#include "Ren_Low_Poly_Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


ALowPoly_Survival_GameMode::ALowPoly_Survival_GameMode()
{

    //SpawnDelay = 5.0f;
    RoundDelay = 2.5f;
    BaseEnemiesPerRound = 4;
    SpawnRadius = 3500.0f;
    CurrentRound = 1;
    AdditionalEnemyHealthPerRound = 50.0f;
   

}

void ALowPoly_Survival_GameMode::BeginPlay()
{

    Super::BeginPlay();

    StartNextRound();

}

void ALowPoly_Survival_GameMode::Tick(float DeltaTime)
{

    //StartNextRound();

}

void ALowPoly_Survival_GameMode::SpawnEnemies()
{

    if (EnemyClass == nullptr) return;

    UWorld* World = GetWorld();
    if (World == nullptr) return;

    // Calculate the number of enemies to spawn this round
    int32 EnemiesToSpawn = BaseEnemiesPerRound + (CurrentRound - 1) * AdditionalEnemiesPerRound;

    for (int32 i = 0; i < EnemiesToSpawn; i++)
    {
        FVector SpawnLocation = GetRandomPointNearPlayer();
        FRotator SpawnRotation = FRotator::ZeroRotator;

        AEnemy_Poly* SpawnedEnemy = World->SpawnActor<AEnemy_Poly>(EnemyClass, SpawnLocation, SpawnRotation);
        if (SpawnedEnemy)
        {
            // Set the enemy's health based on the current round
            float EnemyHealth = BaseEnemyHealth + (CurrentRound - 1) * AdditionalEnemyHealthPerRound;
            EnemyHealth = SpawnedEnemy->CurrentEnemyHealth;
            EnemyHealth = SpawnedEnemy->MaxEnemyHealth;

            

            // Add to the list of spawned enemies
            SpawnedEnemies.Add(SpawnedEnemy);

            // Bind the enemy's destruction delegate
            SpawnedEnemy->bIsDead;
        }
    }


}





void ALowPoly_Survival_GameMode::StartNextRound()
{
    // Clear the list of spawned enemies
    SpawnedEnemies.Empty();

    // Start the spawning timer for the next round
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALowPoly_Survival_GameMode::SpawnEnemies, RoundDelay, false);
}




void ALowPoly_Survival_GameMode::CheckForNextRound()
{
    // Check if all enemies are dead
    bool bAllEnemiesDead = true;
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (Enemy && !Enemy->bIsDead)
        {
            bAllEnemiesDead = false;
            break;
        }
    }

    // If all enemies are dead, start the next round
    if (bAllEnemiesDead)
    {
        CurrentRound++;
        StartNextRound();
    }
}




void ALowPoly_Survival_GameMode::OnEnemyDestroyed()
{

    for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; i--)

    {

        if (SpawnedEnemies[i] && SpawnedEnemies[i]->bIsDead)

        {

            SpawnedEnemies.RemoveAt(i);

        }

    }

}

FVector ALowPoly_Survival_GameMode::GetRandomPointNearPlayer()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return FVector::ZeroVector;

    ACharacter* PlayerCharacter = Cast<ARen_Low_Poly_Character>(PlayerController->GetPawn());
    if (!PlayerCharacter) return FVector::ZeroVector;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();

    // Generate a random point around the player within the specified radius
    float RandomAngle = FMath::RandRange(0.f, 360.f);
    float RandomRadius = FMath::RandRange(0.f, SpawnRadius);

    FVector RandomPoint = PlayerLocation + FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
        FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
        0.f);

    return RandomPoint;

    
}
