// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Enemy_Poly.generated.h"



class ARen_Low_Poly_Character;



UCLASS()
class PROJECT_ATLA_API AEnemy_Poly : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy_Poly();


	//Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float EnemyHealth;

	UFUNCTION(BlueprintCallable, Category = "Health")
		bool EnemyIsDead() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool bIsDead;

	virtual float ApplyDamage(float DamageAmount, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser);


	void Death();
	//Health



	//Enemy Attack & Defence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float DefencePercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float BaseAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float AttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float TotalEnemyAttack;

	UPROPERTY(EditAnywhere, Category = "Attack")
		UAnimMontage* AttackAnimation;

	void PlayAttackAnim();

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void InflictDamageOnCharacter(ARen_Low_Poly_Character* LowPolyRen);
	//Enemy Attack & Defence


	//Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool Attacking;

	//Combat



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
