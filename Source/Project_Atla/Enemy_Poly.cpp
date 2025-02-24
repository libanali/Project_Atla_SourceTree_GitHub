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

	bEnemyIsHit = false;

	bIsDead = false;

	DefencePercentage = 10.0f;

	AttackMultiplier = 1.5f;

	BaseAttack = 10.0f;

	bIsVibrating = false; // Default no vibration
	VibrationIntensity = 5.0f; // Default vibration intensity



	// Initialize the Health Bar Widget Component
	EnemyHealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	EnemyHealthBarWidgetComponent->SetupAttachment(RootComponent);
	EnemyHealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);


	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));

	// Attach directly to the mesh
	AttackCollisionBox->SetupAttachment(GetMesh());
	AttackCollisionBox->SetCollisionProfileName(TEXT("OverlapAll"));
	AttackCollisionBox->SetGenerateOverlapEvents(true);

}


bool AEnemy_Poly::EnemyIsDead() const
{
	return false;
}

void AEnemy_Poly::ResetHurtState()
{
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

	bEnemyIsHit = true;



	PlayAnimMontage(HurtAnimation, 1.0f);




	CurrentEnemyHealth -= DamageAmount;

	if (CurrentEnemyHealth <= 0)

	{

		Death();

	}


	return DamageAmount;
}



void AEnemy_Poly::Death()
{
	if (bIsDead) return;

	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("Enemy %s has died"), *GetName());

	// Notify player character to remove arrow BEFORE destroying the actor
	ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		// This ensures the arrow is removed immediately when the enemy dies
		PlayerCharacter->OnEnemyDestroyed(this);

		// Then process score, XP, etc.
		ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			int32 PointsEarned = GameMode->CalculatePointsForEnemy(Enemy_Score);
			PlayerCharacter->AddPoints(PointsEarned);
			PlayerCharacter->AddWeaponEXP(EXP_Gained);
		}
	}

	// Handle game mode updates
	ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->SpawnedEnemies.Remove(this);

		AEnemy_AIController* AIController = Cast<AEnemy_AIController>(GetController());
		if (AIController)
		{
			GameMode->ActiveEnemies.Remove(AIController);

			if (GameMode->CurrentAttacker == AIController)
			{
				GameMode->ResetAttackCycle();
				GameMode->CycleToNextEnemy();
			}
		}
	}

	AttemptItemDrop();
	Destroy(true);
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
	if (!LowPolyRen || LowPolyRen->bIsDead || LowPolyRen->bIsInvincible) return;  // Check if already invincible

	// Calculate dot product for behind attack detection
	FVector DirectionToPlayer = (GetActorLocation() - LowPolyRen->GetActorLocation());
	FVector PlayerForward = LowPolyRen->GetActorForwardVector();
	float DotResult = FVector::DotProduct(DirectionToPlayer, PlayerForward);

	// Get capsule component
	UCapsuleComponent* PlayerCapsule = LowPolyRen->GetCapsuleComponent();
	if (!PlayerCapsule) return;

	// Store the original material for later
	UMaterialInterface* TheOriginalMaterial = nullptr;
	if (LowPolyRen->GetMesh())
	{
		TheOriginalMaterial = LowPolyRen->GetMesh()->GetMaterial(0);
	}

	// Common damage application function
	auto ApplyDamageAndEffects = [this, LowPolyRen, TheOriginalMaterial]()
	{
		// Calculate and apply damage
		TotalEnemyAttack = BaseAttack * AttackMultiplier;
		float DamageToInflict = TotalEnemyAttack / (1 + LowPolyRen->BaseDefence);
		LowPolyRen->TakeDamage(DamageToInflict);
		LowPolyRen->bIsHurt = true;


		// Apply hurt material
		if (HurtMaterial && LowPolyRen->GetMesh())
		{
			LowPolyRen->GetMesh()->SetMaterial(0, HurtMaterial);
		}

		// Make player invincible
		LowPolyRen->bIsInvincible = true;

		// Set timer to revert changes after 7 seconds
		GetWorld()->GetTimerManager().SetTimer(
			InvincibilityTimerHandle,
			[LowPolyRen, TheOriginalMaterial]()
			{
				// Revert material
				if (TheOriginalMaterial && LowPolyRen->GetMesh())
				{
					LowPolyRen->GetMesh()->SetMaterial(0, TheOriginalMaterial);
				}
				// Remove invincibility
				LowPolyRen->bIsInvincible = false;
			},
			7.0f,  // 7 seconds duration
				false  // don't loop
				);
	};

	if (DotResult < 0.0f)  // Attack from behind
	{
		LowPolyRen->bAttackedFromBehind = true;
		ApplyDamageAndEffects();
		// Play hurt animation with end delegate
		if (LowPolyRen->BehindHurtAnimation)
		{
			UAnimInstance* AnimInstance = LowPolyRen->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(LowPolyRen, &ARen_Low_Poly_Character::OnHurtAnimationEnded);
				AnimInstance->Montage_Play(LowPolyRen->BehindHurtAnimation, 1.4f);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, LowPolyRen->BehindHurtAnimation);
			}
		}
	}
	else  // Attack from front
	{
		LowPolyRen->bAttackedFromBehind = false;
		ApplyDamageAndEffects();

		// Play hurt animation with end delegate
		if (LowPolyRen->HurtAnimation)
		{
			UAnimInstance* AnimInstance = LowPolyRen->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(LowPolyRen, &ARen_Low_Poly_Character::OnHurtAnimationEnded);
				AnimInstance->Montage_Play(LowPolyRen->HurtAnimation, 1.4f);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, LowPolyRen->HurtAnimation);
			}
		}
	}

	// Debug message
	FString AttackDirection = LowPolyRen->bAttackedFromBehind ? TEXT("from behind") : TEXT("from front");
	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red,
		FString::Printf(TEXT("Hit Player %s! Damage: %f"), *AttackDirection, TotalEnemyAttack));
}



void AEnemy_Poly::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{


	// Try to cast the overlapped actor to your player character class
	ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(OtherActor);
	if (PlayerCharacter)
	{
		// Call our comprehensive damage function
		InflictDamageOnCharacter(PlayerCharacter);
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


	// Bind the overlap events
	if (AttackCollisionBox)
	{
		AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Poly::OnAttackOverlapBegin);
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

