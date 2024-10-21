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

    RoundDelay = 2.5f;
    BaseEnemiesPerRound = 3;
    SpawnRadius = 1200.0f;
    CurrentRound = 1;
    AdditionalEnemyHealthPerRound = 40.0f;
    AdditionalEnemiesPerRound = 1.9f;
    BaseSpawnDelay = 2.0f;         // Initial delay between spawns in the first round
    MinSpawnDelay = 1.5f;          // Minimum delay between spawns in later rounds
    DelayDecreasePerRound = 0.1f;

}

void ALowPoly_Survival_GameMode::UpdateEnemyNumbers()
{
    if (SpawnedEnemies.Num() == 0) return;

    // If no enemy is currently attacking, assign one
    if (CurrentAttacker == nullptr || !CurrentAttacker->IsValidLowLevel())
    {
        // Get the first enemy in the list as the new attacker
        AEnemy_AIController* NewAttacker = Cast<AEnemy_AIController>(SpawnedEnemies[0]->GetController());
        if (NewAttacker)
        {
            CurrentAttacker = NewAttacker;
            CurrentAttacker->AttackPlayer();
        }
    }

    // Loop through all enemies
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        AEnemy_AIController* EnemyController = Cast<AEnemy_AIController>(Enemy->GetController());
        if (EnemyController)
        {
            if (EnemyController != CurrentAttacker)
            {
                // Strafe if this enemy is not the current attacker
                EnemyController->StrafeAroundPlayer();
            }
        }
    }
}


void ALowPoly_Survival_GameMode::BeginPlay()
{

    Super::BeginPlay();

    StartNextRound();

    GetWorld()->GetTimerManager().SetTimer(NumberUpdateTimer, this, &ALowPoly_Survival_GameMode::UpdateEnemyNumbers, 5.0f, true);

    CurrentAttacker = nullptr;

    
}



void ALowPoly_Survival_GameMode::Tick(float DeltaTime)
{

    OnEnemyDestroyed();

  //  UpdateEnemyNumbers();

}



void ALowPoly_Survival_GameMode::SpawnEnemies()
{
    // Early return if EnemyClass is not set
    if (EnemyClass == nullptr) return;

    UWorld* World = GetWorld();
    if (World == nullptr) return;

    // Mark that spawning is starting
    bIsSpawningEnemies = true;

    // Determine the number of enemies to spawn for the current round
    int32 EnemiesToSpawn = 4 + (CurrentRound - 1) * AdditionalEnemiesPerRound;
    float LocalSpawnDelay = FMath::Max(MinSpawnDelay, BaseSpawnDelay - (CurrentRound - 1) * DelayDecreasePerRound);

    for (int32 i = 0; i < EnemiesToSpawn; i++)
    {
        FTimerHandle LocalSpawnTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(LocalSpawnTimerHandle, [this, i, EnemiesToSpawn]()
            {
                FVector SpawnLocation = GetRandomPointNearPlayer();
                FRotator SpawnRotation = FRotator::ZeroRotator;

                // Add a small offset to the spawn location based on the enemy index to space them out
                float SpawnOffset = 200.0f; // Adjust this value to control spacing
                SpawnLocation.X += i * SpawnOffset;
                SpawnLocation.Y += i * SpawnOffset;

                TSubclassOf<AEnemy_Poly> EnemyToSpawnClass = GetEnemyClassForCurrentRound();

                if (EnemyToSpawnClass)
                {
                    AEnemy_Poly* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy_Poly>(EnemyToSpawnClass, SpawnLocation, SpawnRotation);
                    if (SpawnedEnemy)
                    {
                        UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s"), *SpawnedEnemy->GetName());

                        float AddedEnemyHealth = SpawnedEnemy->MaxEnemyHealth + (CurrentRound - 1) * AdditionalEnemyHealthPerRound;
                        SpawnedEnemy->IncreaseEnemyHealth(AddedEnemyHealth, true);

                        SpawnedEnemies.Add(SpawnedEnemy);
                    }
                }

                if (i == EnemiesToSpawn - 1)
                {
                    bIsSpawningEnemies = false;

                    ARen_Low_Poly_Character* PlayerChar = Cast<ARen_Low_Poly_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());

                    if (PlayerChar)

                    {

                        float HealthBonus = 3.0f;
                        float AttackBonus = 2.0f;
                        float DefenceBonus = 2.5f;

                        PlayerChar->IncreaseStats(HealthBonus, AttackBonus, DefenceBonus);

                    }

                }

        }, i * LocalSpawnDelay, false);


    }

}




void ALowPoly_Survival_GameMode::StartNextRound()
{
    // Clear the list of spawned enemies
    SpawnedEnemies.Empty();

    // Start the spawning timer for the next round
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALowPoly_Survival_GameMode::SpawnEnemies, RoundDelay, false);

    bIsSpawningEnemies = true;

}




void ALowPoly_Survival_GameMode::CheckForNextRound()
{
    bool bAllEnemiesDead = true;
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (Enemy && !Enemy->bIsDead)
        {
            bAllEnemiesDead = false;
            break;
        }
    }

    if (bAllEnemiesDead && !bIsSpawningEnemies)
    {
        // Reset the attack cycle before moving to the next round
        ResetAttackCycle();

        // Move to the next round
        CurrentRound++;
        StartNextRound();
    }
}





TSubclassOf<AEnemy_Poly> ALowPoly_Survival_GameMode::GetEnemyClassForCurrentRound()
{
    // For rounds 1-4, only spawn spiders
    if (CurrentRound >= 1 && CurrentRound <= 4)
    {
        return BP_Spider;  // Reference to BP_Spider blueprint
    }

    // For rounds 5-9, spawn spiders and wolves
    if (CurrentRound >= 5 && CurrentRound <= 9)
    {
        return FMath::RandBool() ? BP_Spider : BP_Wolf;  // Randomly choose between Spider and Wolf
    }

    // For rounds 10 and above, spawn spiders, wolves, and rock trolls
    if (CurrentRound >= 10)
    {
        int32 RandomChoice = FMath::RandRange(0, 2);
        switch (RandomChoice)
        {
        case 0:
            return BP_Spider;
        case 1:
            return BP_Wolf;
        case 2:
            return BP_RockTroll;
        default:
            return BP_Spider;
        }
    }

    // Default to spawning spiders if something goes wrong
    return BP_Spider;
}

void ALowPoly_Survival_GameMode::OnEnemyDestroyed()
{

    for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; i--)
    {
        if (SpawnedEnemies.IsValidIndex(i) && SpawnedEnemies[i] && SpawnedEnemies[i]->bIsDead)
        {
            SpawnedEnemies.RemoveAt(i);
        }
    }

    // After removing dead enemies, check for the next round
    CheckForNextRound();

}




FVector ALowPoly_Survival_GameMode::GetRandomPointNearPlayer()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return FVector::ZeroVector;

    ACharacter* PlayerCharacter = Cast<ARen_Low_Poly_Character>(PlayerController->GetPawn());
    if (!PlayerCharacter) return FVector::ZeroVector;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem) return FVector::ZeroVector;

    FNavLocation ProjectedLocation;
    FVector RandomPoint;
    bool bIsOnNavMesh = false;

    const int32 MaxAttempts = 10;
    int32 AttemptCount = 0;

    const float MinZValue = PlayerLocation.Z - 50.f;  // Set reasonable Z boundaries
    const float MaxZValue = PlayerLocation.Z + 50.f;  // Avoid spawning too high above the player
    const float MaxTraceDistance = 5000.0f; // Max distance to trace downwards to find ground

    while (!bIsOnNavMesh && AttemptCount < MaxAttempts)
    {
        float RandomAngle = FMath::RandRange(0.f, 360.f);
        float RandomRadius = FMath::RandRange(0.f, SpawnRadius);

        RandomPoint = PlayerLocation + FVector(
            FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
            FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
            0.f  // Z will be adjusted later
        );

        // Try to project to the navmesh
        bIsOnNavMesh = NavSystem->ProjectPointToNavigation(
            RandomPoint,
            ProjectedLocation,
            FVector(500.f, 500.f, 500.f)
        );

        if (bIsOnNavMesh)
        {
            FVector FinalSpawnLocation = ProjectedLocation.Location;

            // Perform a downward trace to find the ground level
            FHitResult HitResult;
            FVector TraceStart = FinalSpawnLocation + FVector(0.f, 0.f, MaxTraceDistance); // Start trace from high above
            FVector TraceEnd = FinalSpawnLocation - FVector(0.f, 0.f, MaxTraceDistance); // Trace down to find ground
            FCollisionQueryParams TraceParams;
            TraceParams.AddIgnoredActor(PlayerCharacter);  // Ignore player in the trace

            // Perform the trace
            bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

            if (bHit)
            {
                // Set the Z value to the ground level from the hit
                FinalSpawnLocation.Z = HitResult.Location.Z;

                // Ensure the spawn location isn't too far above or below the player
                FinalSpawnLocation.Z = FMath::Clamp(FinalSpawnLocation.Z, MinZValue, MaxZValue);

                return FinalSpawnLocation; // Return valid spawn point
            }
        }

        AttemptCount++;
    }

    // Fallback: return player location if no valid NavMesh point was found
    return PlayerLocation;
    
}




void ALowPoly_Survival_GameMode::AssignEnemyNumbers()
{

    // Remove any destroyed enemies from the ActiveEnemies array
    ActiveEnemies.RemoveAll([](AEnemy_AIController* EnemyAI) {
        return !EnemyAI || !EnemyAI->GetPawn() || EnemyAI->GetPawn()->IsPendingKillPending();
        });

    int32 MaxNumber = ActiveEnemies.Num();

    // Shuffle or sort enemies randomly
    ActiveEnemies.Sort([](const AEnemy_AIController& A, const AEnemy_AIController& B) {
        return FMath::RandBool();
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

void ALowPoly_Survival_GameMode::CycleToNextEnemy()
{

    // Ensure the current attacker is valid and hasn't been destroyed
    if (CurrentAttacker && CurrentAttacker->GetPawn())
    {
        int32 CurrentAttackerIndex = SpawnedEnemies.IndexOfByKey(CurrentAttacker->GetPawn());

        if (CurrentAttackerIndex != INDEX_NONE)
        {
            // Cycle to the next attacker, limit the number of attempts to avoid infinite recursion
            int32 NumAttempts = 0;
            int32 NextAttackerIndex;
            AEnemy_AIController* NextAttacker;

            do
            {
                NextAttackerIndex = (CurrentAttackerIndex + 1) % SpawnedEnemies.Num();
                NextAttacker = Cast<AEnemy_AIController>(SpawnedEnemies[NextAttackerIndex]->GetController());

                // Break if we find a valid attacker
                if (NextAttacker && NextAttacker->GetPawn())
                {
                    CurrentAttacker = NextAttacker;
                    CurrentAttacker->AttackPlayer();
                    return; // Exit the function once we find a valid attacker
                }

                CurrentAttackerIndex = NextAttackerIndex;
                NumAttempts++;

            } while (NumAttempts < SpawnedEnemies.Num());  // Avoid infinite recursion by limiting to number of enemies

            // If no valid attackers are found after looping through all enemies, reset the cycle
            ResetAttackCycle();
        }
    }
    else
    {
        // If the current attacker is destroyed, try cycling to the next one
        ResetAttackCycle();
    }

}

void ALowPoly_Survival_GameMode::ResetAttackCycle()
{

    // Reset the current attacker
    CurrentAttacker = nullptr;


}

void ALowPoly_Survival_GameMode::StartEnemyAttackCycle()
{

    if (!bIsSpawningEnemies && SpawnedEnemies.Num() > 0)
    {
        // Start the cycle since all enemies have been spawned
        CycleToNextEnemy();  // Call your logic for cycling enemies here
    }


}
