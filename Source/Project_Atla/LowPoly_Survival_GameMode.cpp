// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_Survival_GameMode.h"
#include "Enemy.h"
#include "Ren_Low_Poly_Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Enemy_Token_Manager.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"


ALowPoly_Survival_GameMode::ALowPoly_Survival_GameMode()
{

    //SpawnDelay = 5.0f;
    RoundDelay = 2.5f;
    BaseEnemiesPerRound = 3;
    SpawnRadius = 800.0f;
    CurrentRound = 1;
    AdditionalEnemyHealthPerRound = 40.0f;
    AdditionalEnemiesPerRound = 1.9f;
    BaseSpawnDelay = 2.0f;         // Initial delay between spawns in the first round
    MinSpawnDelay = 1.5f;          // Minimum delay between spawns in later rounds
    DelayDecreasePerRound = 0.1f;

}

void ALowPoly_Survival_GameMode::BeginPlay()
{

    Super::BeginPlay();

    StartNextRound();

    TokenManager = GetWorld()->SpawnActor<AEnemy_Token_Manager>(AEnemy_Token_Manager::StaticClass());

    if (TokenManager)
    {
        UE_LOG(LogTemp, Log, TEXT("Token Manager found and assigned."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Token Manager is null!"));
    }
}

void ALowPoly_Survival_GameMode::Tick(float DeltaTime)
{

    OnEnemyDestroyed();


}


void ALowPoly_Survival_GameMode::SpawnEnemies()
{

    if (EnemyClass == nullptr) return;

    UWorld* World = GetWorld();
    if (World == nullptr) return;

    // Calculate the number of enemies to spawn this round
    int32 EnemiesToSpawn = BaseEnemiesPerRound + (CurrentRound - 1) * AdditionalEnemiesPerRound;

    // Calculate the spawn delay (decrease as the rounds progress)
    float LocalSpawnDelay = FMath::Max(MinSpawnDelay, BaseSpawnDelay - (CurrentRound - 1) * DelayDecreasePerRound);

    // Spawn enemies one by one with delay
    for (int32 i = 0; i < EnemiesToSpawn; i++)
    {
        FTimerHandle LocalSpawnTimerHandle;
        // Use a lambda function to handle the delayed enemy spawn
        GetWorld()->GetTimerManager().SetTimer(LocalSpawnTimerHandle, [this, i]()
            {
                FVector SpawnLocation = GetRandomPointNearPlayer();
                FRotator SpawnRotation = FRotator::ZeroRotator;

                AEnemy_Poly* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy_Poly>(EnemyClass, SpawnLocation, SpawnRotation);

                if (SpawnedEnemy)
                {
                    // Log the spawned enemy
                    UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s"), *SpawnedEnemy->GetName());

                    // Set the enemy's health based on the current round
                    float AddedEnemyHealth = SpawnedEnemy->MaxEnemyHealth + (CurrentRound - 1) * AdditionalEnemyHealthPerRound;
                    SpawnedEnemy->IncreaseEnemyHealth(AddedEnemyHealth, true);

                    // Add to the list of spawned enemies
                    SpawnedEnemies.Add(SpawnedEnemy);

                    // Register the spawned enemy with the token manager (with a small delay to ensure controller is set)
                    FTimerHandle ControllerCheckHandle;
                    GetWorld()->GetTimerManager().SetTimer(ControllerCheckHandle, [this, SpawnedEnemy]()
                        {
                            if (TokenManager)
                            {
                                AEnemy_AIController* EnemyController = SpawnedEnemy->GetController<AEnemy_AIController>();
                                if (EnemyController)
                                {
                                    UE_LOG(LogTemp, Log, TEXT("Registered enemy controller: %s"), *EnemyController->GetName());
                                    TokenManager->RegisterEnemy(EnemyController);
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Warning, TEXT("Enemy controller is null for %s"), *SpawnedEnemy->GetName());
                                }
                            }
                            else
                            {
                                UE_LOG(LogTemp, Warning, TEXT("TokenManager is null!"));
                            }
                        }, 0.1f, false);  // Delay before checking the AI controller

                }
            }, i * LocalSpawnDelay, false);  // Set delay for each spawn
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

        if (SpawnedEnemies.IsValidIndex(i) && SpawnedEnemies[i] && SpawnedEnemies[i]->bIsDead)

        {

            SpawnedEnemies.RemoveAt(i);

        }

        CheckForNextRound();

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
