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

	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		[this]()
		{
			if (IsValid(this))
			{
				Destroy();
			}
		},
		0.5f,  // Increased from 0.1f to 0.5f
			false
			);
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
	
	// Initial checks
	if (!LowPolyRen || LowPolyRen->bIsDead || LowPolyRen->bIsInvincible) return;

	// Calculate dot product for behind attack detection
	FVector DirectionToPlayer = (GetActorLocation() - LowPolyRen->GetActorLocation());
	FVector PlayerForward = LowPolyRen->GetActorForwardVector();
	float DotResult = FVector::DotProduct(DirectionToPlayer, PlayerForward);

	// Set attacked from behind BEFORE any damage or interruption
	LowPolyRen->bAttackedFromBehind = (DotResult < 0.0f);

	// Get capsule component
	UCapsuleComponent* PlayerCapsule = LowPolyRen->GetCapsuleComponent();
	if (!PlayerCapsule) return;

	// Store the original material for later
	UMaterialInterface* TheOriginalMaterial = nullptr;
	if (LowPolyRen->GetMesh())
	{
		TheOriginalMaterial = LowPolyRen->GetMesh()->GetMaterial(0);
	}

	// Calculate damage
	TotalEnemyAttack = BaseAttack * AttackMultiplier;
	float DamageToInflict = TotalEnemyAttack / (1 + LowPolyRen->BaseDefence);

	// Save current health for comparison
	float CurrentHealth = LowPolyRen->HealthStruct.CurrentHealth;

	// Apply damage - this will interrupt current animation without playing hurt animation
	LowPolyRen->TakeDamage(DamageToInflict);

	// Check if damage was fatal
	if (LowPolyRen->bIsDead || LowPolyRen->HealthStruct.CurrentHealth <= 0.0f)
	{
		// Player died from this hit, don't apply hurt effects or animations
		GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red,
			FString::Printf(TEXT("Fatal Hit on Player! Damage: %f"), TotalEnemyAttack));
		return;
	}

	// Set player in hurt state
	LowPolyRen->bIsHurt = true;

	// Apply hurt material
	if (HurtMaterial && LowPolyRen->GetMesh())
	{
		LowPolyRen->GetMesh()->SetMaterial(0, HurtMaterial);
	}

	// Make player invincible
	LowPolyRen->bIsInvincible = true;

	// Force play appropriate hurt animation with a small delay to ensure previous animations are fully stopped
	UAnimMontage* HurtAnim = LowPolyRen->bAttackedFromBehind ?
		LowPolyRen->BehindHurtAnimation : LowPolyRen->HurtAnimation;

	if (HurtAnim)
	{
		// Use a timer to ensure previous animation is completely stopped
		FTimerHandle HurtAnimTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			HurtAnimTimerHandle,
			[this, LowPolyRen, HurtAnim]()
			{
				if (LowPolyRen && !LowPolyRen->bIsDead)
				{
					// Ensure we're not in a broken state
					APlayerController* PlayerController = Cast<APlayerController>(LowPolyRen->GetController());
					if (PlayerController)
					{
						PlayerController->EnableInput(PlayerController);
					}

					UAnimInstance* AnimInstance = LowPolyRen->GetMesh()->GetAnimInstance();
					if (AnimInstance)
					{
						// Stop any animations that might have started during the delay
						AnimInstance->StopAllMontages(0.0f);

						FOnMontageEnded EndDelegate;
						EndDelegate.BindUObject(LowPolyRen, &ARen_Low_Poly_Character::OnHurtAnimationEnded);
						AnimInstance->Montage_Play(HurtAnim, 1.4f);
						AnimInstance->Montage_SetEndDelegate(EndDelegate, HurtAnim);

						UE_LOG(LogTemp, Warning, TEXT("Playing Hurt Animation: %s (from behind: %s)"),
							*HurtAnim->GetName(),
							LowPolyRen->bAttackedFromBehind ? TEXT("Yes") : TEXT("No"));
					}
				}
			},
			0.02f,
				false
				);
	}

	// Set timer to revert changes after 5 seconds
	GetWorld()->GetTimerManager().SetTimer(
		InvincibilityTimerHandle,
		[LowPolyRen, TheOriginalMaterial]()
		{
			if (LowPolyRen && !LowPolyRen->bIsDead)
			{
				// Revert material
				if (TheOriginalMaterial && LowPolyRen->GetMesh())
				{
					LowPolyRen->GetMesh()->SetMaterial(0, TheOriginalMaterial);
				}
				// Remove invincibility
				LowPolyRen->bIsInvincible = false;
			}
		},
		5.0f,  // 5 seconds duration
			false  // don't loop
			);

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
 // Save enemy location and rotation before any destruction
	FVector DropLocation = GetActorLocation();
	FRotator DropRotation = FRotator::ZeroRotator;
	UWorld* World = GetWorld();

	if (!World) return;  // Safety check

	// Determine if an item should drop with a 45% chance
	float DropRoll = FMath::FRand();
	if (DropRoll > 0.45f) return;

	// Calculate total probabilities
	float TotalProbability = 0.f;
	for (const FItemDrop& ItemDrop : PossibleItemDrops)
	{
		TotalProbability += ItemDrop.DropChance;
	}

	DropRoll = FMath::FRand();
	if (DropRoll < TotalProbability)
	{
		float CumulativeProbability = 0.f;
		for (const FItemDrop& ItemDrop : PossibleItemDrops)
		{
			CumulativeProbability += ItemDrop.DropChance;
			if (DropRoll <= CumulativeProbability && ItemDrop.ItemClass)
			{
				// Spawn the item using cached location
				AActor* SpawnedItem = World->SpawnActor<AActor>(
					ItemDrop.ItemClass,
					DropLocation,
					DropRotation
				);

				// Log and verify spawn
				if (SpawnedItem)
				{
					UE_LOG(LogTemp, Warning, TEXT("Item spawned successfully: %s"), *SpawnedItem->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn item of class %s"),
						ItemDrop.ItemClass ? *ItemDrop.ItemClass->GetName() : TEXT("NULL"));
				}
				break;
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

