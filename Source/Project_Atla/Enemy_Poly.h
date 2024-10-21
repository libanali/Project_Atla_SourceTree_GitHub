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
#include "Enemy_Token_Manager.h"
#include "Enemy_Poly.generated.h"



class ARen_Low_Poly_Character;
class AEnemy_AIController;



UCLASS()
class PROJECT_ATLA_API AEnemy_Poly : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy_Poly();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Drops")
		TArray<FItemDrop> PossibleItemDrops;


	//Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentEnemyHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float MaxEnemyHealth;

	UFUNCTION(BlueprintCallable, Category = "Health")
		bool EnemyIsDead() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool bIsDead;

	UFUNCTION(BlueprintCallable)
		void IncreaseEnemyHealth(float Amount, bool bSetInitialHealth = false);

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

	UFUNCTION(BlueprintCallable)
		void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		UAnimMontage* AttackAnimation;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void InflictDamageOnCharacter(ARen_Low_Poly_Character* LowPolyRen);
	//Enemy Attack & Defence


	//Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool Attacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool IsPetrified;

	UPROPERTY()
		AEnemy_Token_Manager* TokenManager;

	bool bHasToken;
	//Combat





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



private:


};
