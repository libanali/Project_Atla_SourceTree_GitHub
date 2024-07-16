// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Enemy_Poly.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float SpawnRadius;

    UFUNCTION()
        void OnEnemyDestroyed();

    FVector GetRandomPointNearPlayer();
	
};
