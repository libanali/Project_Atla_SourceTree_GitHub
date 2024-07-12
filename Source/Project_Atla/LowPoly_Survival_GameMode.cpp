// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_Survival_GameMode.h"
<<<<<<< HEAD
<<<<<<< HEAD
#include "Enemy_Poly.h"
#include "Ren_Low_Poly_Character.h"

=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)

ALowPoly_Survival_GameMode::ALowPoly_Survival_GameMode()

{

<<<<<<< HEAD
<<<<<<< HEAD
	CurrentRound = 1;
	EnemiesPerWave = 0;
	bGameOver = false;
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)



}

void ALowPoly_Survival_GameMode::BeginPlay()
{



}

void ALowPoly_Survival_GameMode::Tick(float DeltaTime)
{
}

void ALowPoly_Survival_GameMode::StartNewRound()
{
<<<<<<< HEAD
<<<<<<< HEAD

	EnemiesSpawned = 0;
	EnemiesDefeated = 0;

	EnemiesPerWave += CurrentRound;

	//SpawnEnemies();

=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
}

void ALowPoly_Survival_GameMode::SpawnEnemies()
{
<<<<<<< HEAD
<<<<<<< HEAD

	for (int32 i = 0; i < EnemiesPerWave; ++i)

	{

		FVector SpawnLocation = GetRandomSpawnLocation();

		AEnemy_Poly* Enemy = GetWorld()->SpawnActor<AEnemy_Poly>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);

		//how about trying all actors of class?


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

=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
}

void ALowPoly_Survival_GameMode::EndGame()
{
<<<<<<< HEAD
<<<<<<< HEAD

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

	float SpawnRadius = 5000.0f;


	FVector RandomDirection = FMath::VRand();
	RandomDirection.Z = 0.0f;
	FVector RandomPoint = PlayerLocation + RandomDirection * FMath::FRandRange(0.0f, SpawnRadius);


	return RandomPoint;
<<<<<<< HEAD
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
=======
>>>>>>> parent of 5bae13e (Enemies can now  spawn in level)
}








=======
}

>>>>>>> parent of 025d3f7 (Made some minor changes and added new C++ ai controller)





