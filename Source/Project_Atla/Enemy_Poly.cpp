// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Poly.h"
#include "Ren_Low_Poly_Character.h"

// Sets default values
AEnemy_Poly::AEnemy_Poly()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Attacking = false;

	//EnemyHealth = 100.0f;

	bIsDead = false;

	DefencePercentage = 3.0f;

	AttackMultiplier = 1.5f;

	BaseAttack = 5.0f;

}

bool AEnemy_Poly::EnemyIsDead() const
{
	return false;
}

float AEnemy_Poly::ApplyDamage(float DamageAmount, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser)
{

	float CalculatedDamage = DamageAmount * (1 - DefencePercentage);

	EnemyHealth -= CalculatedDamage;

	return CalculatedDamage;
}



void AEnemy_Poly::Death()
{

	if (EnemyHealth <= 0)

	{

		bIsDead = true;

		Destroy(true);

	}

}


void AEnemy_Poly::PlayAttackAnim()
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

// Called when the game starts or when spawned
void AEnemy_Poly::BeginPlay()
{
	Super::BeginPlay();

	EnemyHealth = 400.0f;

	
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

