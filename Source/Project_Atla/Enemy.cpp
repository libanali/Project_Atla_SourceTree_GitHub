// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Ren_Character.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyHealth = 600.0f;

	bIsDead = false;

	DefencePercentage = 3.0f;

	AttackMultiplier = 1.5f;

	BaseAttack = 5.0f;

}

bool AEnemy::EnemyIsDead() const
{
	return false;
}

void AEnemy::Death()
{


	if (EnemyHealth <= 0)

	{

		Destroy();

		bIsDead = true;

		ARen_Character* Ren_Character = Cast<ARen_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());

		//Ren_Character->GainExperience(40.f);

		if(Ren_Character)
		{

			Ren_Character->TurnOffHardLock();
			Ren_Character->ToggleHardLockOnNextEnemy();

		}


	}



}

void AEnemy::InflictDamageOnCharacter(ARen_Character* RenCharacter)
{


	if (RenCharacter)

	{

		TotalEnemyAttack = BaseAttack * AttackMultiplier;
		float DamageToInflict = TotalEnemyAttack / (1 + RenCharacter->BaseDefence);

		RenCharacter->ReceiveDamage(DamageToInflict);

	}




}

float AEnemy::ApplyDamage(float DamageAmount, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser)
{

	float CalculatedDamage = DamageAmount * (1 - DefencePercentage);

	EnemyHealth -= CalculatedDamage;

	return CalculatedDamage;
}


// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Death();

}

