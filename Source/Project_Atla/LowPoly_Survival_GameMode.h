// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Enemy_Poly.h"
#include "Enemy_Token_Manager.h"
#include "LowPoly_Survival_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API ALowPoly_Survival_GameMode : public AGameMode
{
	GENERATED_BODY()


public:
    ALowPoly_Survival_GameMode();

    void UpdateEnemyNumbers();

    AEnemy_AIController* CurrentAttacker;

    void CycleToNextEnemy();



protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    // Function to spawn enemies
    void SpawnEnemies();

    // Function to start the next round
    void StartNextRound();

    // Check if all enemies are destroyed
    void CheckForNextRound();

    // Timer handle to manage spawning rounds
    FTimerHandle SpawnTimerHandle;

    // Delay between each spawn round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float SpawnDelay;

    // Enemy class to spawn
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
        TSubclassOf<AEnemy_Poly> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float RoundDelay;

    // Base number of enemies to spawn each round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        int32 BaseEnemiesPerRound;

    // Number of additional enemies per round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round")
        int32 AdditionalEnemiesPerRound;

    // Current round number
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round")
    int32 CurrentRound;

    // Base health for enemies
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float BaseEnemyHealth;

    // Additional health per round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float AdditionalEnemyHealthPerRound;

    // List of currently spawned enemies
    TArray<AEnemy_Poly*> SpawnedEnemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawning")
        float BaseSpawnDelay;  // Initial delay between spawns in the first round

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawning")
        float MinSpawnDelay;   // Minimum delay between spawns

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawning")
        float DelayDecreasePerRound;  // How much the delay decreases each round

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float SpawnRadius;

    // List of active enemy controllers
    TArray<AEnemy_AIController*> ActiveEnemies;



    UFUNCTION()
        void OnEnemyDestroyed();

    FVector GetRandomPointNearPlayer();





private:

    // Function to assign numbers to enemies
    void AssignEnemyNumbers();

    // Timer for updating enemy numbers
    FTimerHandle NumberUpdateTimer;


	
};
