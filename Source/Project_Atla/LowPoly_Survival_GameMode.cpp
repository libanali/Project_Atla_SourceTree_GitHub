// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_Survival_GameMode.h"
#include "Enemy_Poly.h"
#include "Ren_Low_Poly_Character.h"
#include "NavigationSystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ALowPoly_Survival_GameMode::ALowPoly_Survival_GameMode()

{

	CurrentRound = 1;
	EnemiesPerWave = 5;
	bGameOver = false;

}



void ALowPoly_Survival_GameMode::BeginPlay()
{

	StartNewRound();

}



void ALowPoly_Survival_GameMode::Tick(float DeltaTime)
{
	{

		if (ShouldEndGame())

		{

			EndGame();
		}

		CheckEnemiesStatus();

	}


}



void ALowPoly_Survival_GameMode::StartNewRound()
{

	EnemiesSpawned = 0;
	EnemiesDefeated = 0;

	EnemiesPerWave += CurrentRound;

	SpawnEnemies();

}



void ALowPoly_Survival_GameMode::SpawnEnemies()
{

	for (int32 i = 0; i < EnemiesPerWave; ++i)

	{

		FVector SpawnLocation = GetRandomSpawnLocation();

		AEnemy_Poly* Enemy = GetWorld()->SpawnActor<AEnemy_Poly>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);


		if (Enemy)
		{

			SpawnedEnemies.Add(Enemy);
			EnemiesSpawned++;

		}

	}


}



void ALowPoly_Survival_GameMode::HandlePlayerDefeat()
{

	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("defeated!!"));


}



void ALowPoly_Survival_GameMode::CheckEnemiesStatus()
{

	for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; --i)
	{

		if (SpawnedEnemies[i] && SpawnedEnemies[i]->bIsDead)

		{

			EnemiesDefeated++;
			SpawnedEnemies.RemoveAt(i);

		}

	}

	if (EnemiesDefeated >= EnemiesSpawned)

	{

		StartNewRound();

	}

}


void ALowPoly_Survival_GameMode::EndGame()
{

	bGameOver = true;
	HandlePlayerDefeat();

}



bool ALowPoly_Survival_GameMode::ShouldEndGame()
{

	ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(this, 0));

	return Ren && Ren->bIsDead;
}



FVector ALowPoly_Survival_GameMode::GetRandomSpawnLocation()
{

	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	float SpawnRadius = 4500.0f;


	FVector RandomDirection = FMath::VRand();
	RandomDirection.Z = 0.0f;
	FVector RandomPoint = PlayerLocation + RandomDirection * FMath::FRandRange(0.0f, SpawnRadius);



	return RandomPoint;
}

FVector ALowPoly_Survival_GameMode::GetRandomSpawnInNavMesh()
{
	
	return FVector();
}










