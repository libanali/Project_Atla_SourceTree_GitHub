// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy_Character_Class.generated.h"

class ARen_Character;
class UAnimMontage;



UCLASS()
class PROJECT_ATLA_API AEnemy_Character_Class : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy_Character_Class();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float EnemyHealth;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float DefencePercentage;


	UFUNCTION(BlueprintCallable, Category = "Enemy")
		bool EnemyIsDead() const;






	bool bIsDead;

	void Death();





	UFUNCTION(BlueprintCallable, Category = "Damage")
		void InflictDamageOnCharacter(ARen_Character* RenCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* HitReaction;




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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
