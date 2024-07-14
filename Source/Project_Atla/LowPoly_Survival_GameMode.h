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

    // Function to spawn enemies
    void SpawnEnemies();

    // Timer handle to manage spawning rounds
    FTimerHandle SpawnTimerHandle;

    // Delay between each spawn round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float SpawnDelay;

    // Number of enemies to spawn each round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        int32 EnemiesPerRound;

    // Enemy class to spawn
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
        TSubclassOf<AEnemy_Poly> EnemyClass;




    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float SpawnRadius;

    FVector GetRandomPointNearPlayer();


	
};
