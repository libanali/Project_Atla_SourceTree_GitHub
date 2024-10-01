// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Poly.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "LowPoly_Survival_GameMode.h"



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

	return CalculatedDamage;
}



void AEnemy_Poly::Death()
{

	if (CurrentEnemyHealth <= 0)

	{

		bIsDead = true;
		Destroy(true);

		ARen_Low_Poly_Character* LowPoly_Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		LowPoly_Ren->GainExperience(25);
	}

}


void AEnemy_Poly::Attack()
{

	PlayAnimMontage(AttackAnimation, 1.0f);

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

void AEnemy_Poly::RequestToken()
{




}



void AEnemy_Poly::ReleaseToken()
{




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
		if (GameMode && GameMode->TokenManager)
		{
			GameMode->TokenManager->RegisterEnemy(this->GetController<AEnemy_AIController>());
		}
	}

}

// Called every frame
void AEnemy_Poly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	Death();
	

}

// Called to bind functionality to input
void AEnemy_Poly::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

