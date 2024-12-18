// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Poly.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "LowPoly_Survival_GameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Enemy_Poly_HealthBar_Widget.h"
#include "Components/WidgetComponent.h"




// Sets default values
AEnemy_Poly::AEnemy_Poly()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//MaxEnemyHealth = 100.0f;

	Attacking = false;

	CurrentEnemyHealth = MaxEnemyHealth;

	bIsDead = false;

	DefencePercentage = 30.0f;

	AttackMultiplier = 1.5f;

	BaseAttack = 10.0f;

	bIsVibrating = false; // Default no vibration
	VibrationIntensity = 5.0f; // Default vibration intensity



	// Initialize the Health Bar Widget Component
	EnemyHealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	EnemyHealthBarWidgetComponent->SetupAttachment(RootComponent);
	EnemyHealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

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

	//float CalculatedDamage = DamageAmount * (1 - DefencePercentage);

	// Check if the Enemy is valid
	if (!this)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemy_Poly is invalid (this is null)"));
		return 0.0f;
	}

	UpdateHealthBar();


	CurrentEnemyHealth -= DamageAmount;

	if (CurrentEnemyHealth <= 0)

	{

		Death();

	}


	return DamageAmount;
}



void AEnemy_Poly::Death()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	// Log enemy death for debugging purposes
	UE_LOG(LogTemp, Warning, TEXT("Enemy %s has died"), *GetName());

	// Get reference to the game mode to handle enemy removal and score calculation
	ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		// Calculate points for this enemy
		int32 PointsEarned = GameMode->CalculatePointsForEnemy(Enemy_Score);

		// Award points to the player
		ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (PlayerCharacter)
		{
			PlayerCharacter->AddPoints(PointsEarned);
			PlayerCharacter->UpdateHighScore(PointsEarned);
			// Award EXP to the player's currently equipped weapon
			PlayerCharacter->AddWeaponEXP(EXP_Gained);
			PlayerCharacter->QueueEXP(EXP_Gained);

			// Remove the arrow widget for this enemy (check and cleanup)
			if (PlayerCharacter->EnemyArrowMap.Contains(this))
			{
				UEnemy_Detection_Arrow* ArrowWidget = PlayerCharacter->EnemyArrowMap[this];
				if (ArrowWidget)
				{
					ArrowWidget->RemoveFromViewport();  // Remove from screen
					ArrowWidget = nullptr;
				}
				PlayerCharacter->EnemyArrowMap.Remove(this);  // Remove from the map
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ArrowWidget not found for this enemy!"));
			}
		}

		// Remove enemy from any game mode lists
		GameMode->SpawnedEnemies.Remove(this);

		// Handle the AI controller and its removal from active enemies
		AEnemy_AIController* AIController = Cast<AEnemy_AIController>(GetController());
		if (AIController)
		{
			GameMode->ActiveEnemies.Remove(AIController);
		}

		// Ensure the game mode attack cycle is reset if this enemy was attacking
		if (GameMode->CurrentAttacker == AIController)
		{
			GameMode->ResetAttackCycle();
			GameMode->CycleToNextEnemy();
		}
	}

	// Finally, destroy the enemy and attempt item drop if applicable
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








float AEnemy_Poly::GetHealthPercentage() const
{


	return (MaxEnemyHealth > 0) ? (CurrentEnemyHealth / MaxEnemyHealth) : 0.0f;

}




void AEnemy_Poly::UpdateHealthBar()
{



	if (EnemyHealthBarWidgetComponent)
	{
		// Get the widget instance
		UUserWidget* WidgetInstance = EnemyHealthBarWidgetComponent->GetWidget();

		if (WidgetInstance)
		{
			// Cast to your custom widget class
			UEnemy_Poly_HealthBar_Widget* HealthBar = Cast<UEnemy_Poly_HealthBar_Widget>(WidgetInstance);
			if (HealthBar)
			{
				// Update the health bar with the current health percentage
				float HealthPercentage = (MaxEnemyHealth > 0) ? (CurrentEnemyHealth / MaxEnemyHealth) : 0.0f;
				HealthBar->UpdateHealthBar(HealthPercentage);
			}
		}
	}




}







// Called when the game starts or when spawned
void AEnemy_Poly::BeginPlay()
{
	Super::BeginPlay();


	// Register this enemy with the token manager
	if (GetWorld())
	{
		ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
		
	}

	bShouldFacePlayer = true;

	BurnDurationRemaining = 0.0f;

	bIsBurning = false;
	bIsFrozen = false;
	bIsStunned = false;


}



// Called every frame
void AEnemy_Poly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

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

