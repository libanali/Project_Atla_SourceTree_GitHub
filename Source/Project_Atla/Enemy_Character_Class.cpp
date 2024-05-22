// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Character_Class.h"
#include "Ren_Character.h"

// Sets default values
AEnemy_Character_Class::AEnemy_Character_Class()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyHealth = 600.0f;

	bIsDead = false;

	DefencePercentage = 3.0f;

	AttackMultiplier = 1.5f;

	BaseAttack = 5.0f;


}

bool AEnemy_Character_Class::EnemyIsDead() const
{
	return false;
}

void AEnemy_Character_Class::Death()
{

	if (EnemyHealth <= 0)

	{

		Destroy();

		bIsDead = true;

		ARen_Character* Ren_Character = Cast<ARen_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());

		//Ren_Character->GainExperience(40.f);

		if (Ren_Character)
		{

			Ren_Character->TurnOffHardLock();
			Ren_Character->ToggleHardLockOnNextEnemy();

		}


	}



}





void AEnemy_Character_Class::InflictDamageOnCharacter(ARen_Character* RenCharacter)
{

	if (RenCharacter)

	{

		TotalEnemyAttack = BaseAttack * AttackMultiplier;
		float DamageToInflict = TotalEnemyAttack / (1 + RenCharacter->BaseDefence);

		RenCharacter->TakeDamage(DamageToInflict);

	}



}

float AEnemy_Character_Class::ApplyDamage(float DamageAmount, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser)
{

	float CalculatedDamage = DamageAmount * (1 - DefencePercentage);

	EnemyHealth -= CalculatedDamage;

	PlayAnimMontage(HitReaction, 1.0);

	return CalculatedDamage;

}

// Called when the game starts or when spawned
void AEnemy_Character_Class::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy_Character_Class::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Death();

}

// Called to bind functionality to input
void AEnemy_Character_Class::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

