// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDestroyedEvent, AEnemy*, DestroyedEnemy);


class ARen_Character;


UCLASS()

class PROJECT_ATLA_API AEnemy : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	AEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float EnemyHealth;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float DefencePercentage;


	UFUNCTION(BlueprintCallable, Category = "Enemy")
		bool EnemyIsDead() const;

	UPROPERTY(BlueprintAssignable)
		FEnemyDestroyedEvent OnEnemyDestroyed;


	bool bIsDead;

	void Death();



	UFUNCTION(BlueprintCallable, Category = "Damage")
		void InflictDamageOnCharacter(ARen_Character* RenCharacter);


	//virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual float ApplyDamage(float DamageAmount, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser);



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float BaseAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float AttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float TotalEnemyAttack;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
