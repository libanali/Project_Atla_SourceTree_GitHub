// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
<<<<<<< HEAD
<<<<<<< HEAD
#include "Kismet/Gameplaystatics.h"
#include "NavigationSystem.h"
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
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

	void EndGame();


	//UFUNCTION(BlueprintImplementableEvent)
		//void FindRandomLocationInNavMesh();

	

	
protected:

	int32 CurrentRound;
	int32 EnemiesPerWave;



<<<<<<< HEAD
<<<<<<< HEAD
	FVector GetRandomSpawnLocation();


	
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class AEnemy_Poly> EnemyClass;

	UPROPERTY()
		TArray<AEnemy_Poly*> SpawnedEnemies;
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)

};
