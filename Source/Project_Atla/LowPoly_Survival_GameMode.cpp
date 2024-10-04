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

void ALowPoly_Survival_GameMode::UpdateEnemyNumbers()
{

    // Assign numbers to enemies
    AssignEnemyNumbers();

    // Update behavior for each enemy
    for (AEnemy_AIController* EnemyController : ActiveEnemies)
    {
        if (EnemyController)
        {
            EnemyController->UpdateBehavior();
        }
    }


}


void ALowPoly_Survival_GameMode::BeginPlay()
{

    Super::BeginPlay();

    StartNextRound();

    GetWorld()->GetTimerManager().SetTimer(NumberUpdateTimer, this, &ALowPoly_Survival_GameMode::UpdateEnemyNumbers, 5.0f, true);


    
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
    int32 EnemiesToSpawn = 4 + (CurrentRound - 1) * AdditionalEnemiesPerRound;

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

                    // No more TokenManager logic required, so remove it entirely
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




void ALowPoly_Survival_GameMode::AssignEnemyNumbers()
{

    int32 MaxNumber = ActiveEnemies.Num();

    // Shuffle or sort enemies randomly or based on your logic
    ActiveEnemies.Sort([](const AEnemy_AIController& A, const AEnemy_AIController& B) {
        return FMath::RandBool(); // Randomize enemy numbers
        });

    // Assign numbers (1 is the highest priority)
    for (int32 i = 0; i < ActiveEnemies.Num(); i++)
    {
        if (ActiveEnemies[i])
        {
            ActiveEnemies[i]->SetEnemyNumber(MaxNumber - i);  // Highest number first
        }
    }

}
