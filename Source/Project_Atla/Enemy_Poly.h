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
#include "Score_Struct.h"
#include "Enemy_Poly.generated.h"



class ARen_Low_Poly_Character;
class AEnemy_AIController;
class UWidgetComponent;


// Struct to hold item drop information
USTRUCT(BlueprintType)
struct FItemDrop
{
	GENERATED_BODY()

		// Reference to the item Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Drop")
		TSubclassOf<AActor> ItemClass;  // Change this to AActor since your items are Blueprints

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Drop")
		float DropChance;  // Probability of this item dropping (0 to 1)

	// Default constructor
	FItemDrop() : ItemClass(nullptr), DropChance(0.f) {}
};


UCLASS()
class PROJECT_ATLA_API AEnemy_Poly : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy_Poly();


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

	UFUNCTION(BlueprintCallable)
	virtual float ApplyDamage(float DamageAmount, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser);
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		UWidgetComponent* EnemyHealthBarWidgetComponent;

	void UpdateHealthBar();


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

	void StartStunVibration();

	void StopStunVibration();

	void ApplyStunVibrationEffect();




	bool bHasToken;
	bool bShouldFacePlayer;

	bool bIsVibrating; // Flag to track if vibration is active
	FVector OriginalLocation; // Original location of the mesh

	float VibrationTimeElapsed; // Track vibration duration
	float VibrationDuration; // Total time for vibration effect
	float VibrationIntensity; // Intensity of vibration
	//Combat


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UMaterialInterface* OriginalMaterial;

	//Score system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		FScore_Struct Enemy_Score;  // Enemy-specific score data
	//Score system

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		int32 EXP_Gained;



	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetHealthPercentage() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	void AttemptItemDrop();
	void SpawnItem(TSubclassOf<AActor> ItemClass);

};
