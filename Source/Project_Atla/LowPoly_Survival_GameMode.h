// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Kismet/Gameplaystatics.h"
#include "NavigationSystem.h"
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

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;



	void StartNewRound();

	void SpawnEnemies();

	void HandlePlayerDefeat();

	void CheckEnemiesStatus();

	void EndGame();


	//UFUNCTION(BlueprintImplementableEvent)
		//void FindRandomLocationInNavMesh();

	

	
protected:

	int32 CurrentRound;
	int32 EnemiesPerWave;
	int32 EnemiesSpawned;
	int32 EnemiesDefeated;

	FTimerHandle EnemySpawnTimer;

	bool bGameOver;
	bool ShouldEndGame();

	FVector GetRandomSpawnLocation();

	FVector GetRandomSpawnInNavMesh();

	
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class AEnemy_Poly> EnemyClass;

	UPROPERTY()
		TArray<AEnemy_Poly*> SpawnedEnemies;

};
