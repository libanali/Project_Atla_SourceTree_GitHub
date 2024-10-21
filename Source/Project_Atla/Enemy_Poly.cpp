// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Poly.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "LowPoly_Survival_GameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"



// Sets default values
AEnemy_Poly::AEnemy_Poly()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Attacking = false;

	//MaxEnemyHealth = 60.0f;

	CurrentEnemyHealth = MaxEnemyHealth;

	bIsDead = false;

	DefencePercentage = 3.0f;

	AttackMultiplier = 1.5f;

	BaseAttack = 5.0f;


}

bool AEnemy_Poly::EnemyIsDead() const
{
	return false;
}



void AEnemy_Poly::IncreaseEnemyHealth(float Amount, bool bSetInitialHealth)
{

	if (bSetInitialHealth)
	{

		MaxEnemyHealth = Amount;
		CurrentEnemyHealth = Amount;


	}

	else

	{

		CurrentEnemyHealth = FMath::Min(CurrentEnemyHealth + Amount, MaxEnemyHealth);

	}
}



float AEnemy_Poly::ApplyDamage(float DamageAmount, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser)
{

	float CalculatedDamage = DamageAmount * (1 - DefencePercentage);

	CurrentEnemyHealth -= CalculatedDamage;

	if (CurrentEnemyHealth <= 0)

	{

		Death();

	}


	return CalculatedDamage;
}



void AEnemy_Poly::Death()
{
	// If the enemy is already dead, exit early
	if (bIsDead)
	{
		return;
	}

	// Set the enemy as dead
	bIsDead = true;

	// Cast to the game mode to access enemy management
	ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		// Remove this enemy from the SpawnedEnemies list
		GameMode->SpawnedEnemies.Remove(this);

		// Remove this enemy's AIController from the ActiveEnemies list
		AEnemy_AIController* AIController = Cast<AEnemy_AIController>(GetController());
		if (AIController)
		{
			GameMode->ActiveEnemies.Remove(AIController);
		}

		// If this enemy is the current attacker, reset and cycle to the next enemy
		if (GameMode->CurrentAttacker == AIController)
		{
			GameMode->ResetAttackCycle();
			GameMode->CycleToNextEnemy();  // Continue the attack cycle
		}
	}

	// Grant experience to the player character
	ARen_Low_Poly_Character* LowPoly_Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (LowPoly_Ren)
	{
	//	LowPoly_Ren->GainExperience(25);  // Give experience points to the player
	}

	// Destroy the enemy after all necessary actions
	Destroy(true);
	AttemptItemDrop();
}


void AEnemy_Poly::Attack()
{

	if (AttackAnimation)
	{
		PlayAnimMontage(AttackAnimation, 1.0f);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Attack Animation is missing!"));
	}

}



void AEnemy_Poly::InflictDamageOnCharacter(ARen_Low_Poly_Character* LowPolyRen)
{

	if (LowPolyRen)

	{

		TotalEnemyAttack = BaseAttack * AttackMultiplier;
		float DamageToInflict = TotalEnemyAttack / (1 + LowPolyRen->BaseDefence);

		LowPolyRen->TakeDamage(DamageToInflict);

	}

}






// Called when the game starts or when spawned
void AEnemy_Poly::BeginPlay()
{
	Super::BeginPlay();

	CurrentEnemyHealth = 100.0f;

	// Register this enemy with the token manager
	if (GetWorld())
	{
		ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
		
	}

}



// Called every frame
void AEnemy_Poly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//Death();
	

}

void AEnemy_Poly::AttemptItemDrop()
{
	// Determine if an item should drop with a 45% chance
	float DropRoll = FMath::FRand();  // Random float between 0 and 1

	// Check if the item should drop (45% chance)
	if (DropRoll > 0.45f)
	{
		return;  // Exit if the drop check fails
	}

	// If the drop roll passes, proceed to calculate total probabilities for specific item drops
	float TotalProbability = 0.f;

	// Calculate total probabilities for the possible item drops
	for (const FItemDrop& ItemDrop : PossibleItemDrops)
	{
		TotalProbability += ItemDrop.DropChance;  // Access DropChance here
	}

	// If the drop roll is less than the total probability, an item can drop
	DropRoll = FMath::FRand();  // Reroll for item selection

	if (DropRoll < TotalProbability)
	{
		// Randomly select an item based on probabilities
		float CumulativeProbability = 0.f;
		for (const FItemDrop& ItemDrop : PossibleItemDrops)
		{
			CumulativeProbability += ItemDrop.DropChance;  // Access DropChance here
			if (DropRoll <= CumulativeProbability)
			{
				SpawnItem(ItemDrop.ItemClass);  // Access ItemClass here
				break;  // Exit loop after spawning an item
			}
		}
	}
}

void AEnemy_Poly::SpawnItem(TSubclassOf<AActor> ItemClass)
{

	if (ItemClass)
	{
		FVector SpawnLocation = GetActorLocation();  // Spawn at the enemy's location
		FRotator SpawnRotation = FRotator::ZeroRotator;

		// Spawn the item actor
		GetWorld()->SpawnActor<AActor>(ItemClass, SpawnLocation, SpawnRotation);
	}


}

// Called to bind functionality to input
void AEnemy_Poly::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

