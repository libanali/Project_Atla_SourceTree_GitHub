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
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Health_Struct.h"
#include "Mana_Struct.h"
#include "ATB_Struct.h"
#include "Engine/Engine.h"
#include "GameFramework/DamageType.h"
#include "Ren_Character.generated.h"

class AEnemy;

UCLASS()
class PROJECT_ATLA_API ARen_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARen_Character();


	//Character Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraSensitivity;

	//Character and Camera Components



	//Movement Variables
	UFUNCTION(BlueprintCallable)
		void LookUp(float Value);

	UFUNCTION(BlueprintCallable)
		void Turn(float Value);

	void MoveForward(float Axis);
	void MoveRight(float Axis);
	//Movement Variables



	//Health Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
		FHealth_Struct HealthStruct;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void TakeDamage(float DamageAmount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float DamageReduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool bIsDead;

	UFUNCTION(BlueprintCallable)
		void DecreaseHelath();
	//Health Variables



	//Mana Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mana")
		FMana_Struct ManaStruct;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mana")
		float MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mana")
		float CurrentMana;

	UFUNCTION(BlueprintCallable, Category = "Mana")
		void IncreaseMana(float MPAmount);

	UFUNCTION(BlueprintCallable, Category = "Mana")
		void StartManaIncrease();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* IncreaseManaAnimation;
	//Mana Variables


	
	//ATB Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ATB")
		FATB_Struct ATBStruct;
	//ATB Variables




	//Attack Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float BaseAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float AttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float FireAttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float IceAttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float LightningAttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float TotalAttack;

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void CalculateTotalAttack();
	//Attack Variables




	//Defence Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float BaseDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float DefenceMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float TotalDefence;

	UFUNCTION(BlueprintCallable, Category = "Defence")
		void CalculateTotalDefence();
	//Defence Variables



	//Combat Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		bool IsCombatModeOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool Blocking;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void InflictDamageOnEnemy(AEnemy* Enemy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float CalculatedDamage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		//ElementType CurrentElement;


	UFUNCTION(BlueprintCallable)
		void ATB_Increase();

	UFUNCTION(BlueprintCallable)
		void ATB_Tracking();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float CurrentATB;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float MaxATB;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		float ATBFillRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		bool bCanUseAbilityOrItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ATB")
		bool bIsPerformingAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ATB")
		int Current_ATB_Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ATB")
		int Max_ATB_Count;



	float ATB_Attack_Boost;

	//Combat Variables




	//Level Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		int32 CharacterLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		int32 ExperiencePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		TArray<int32> ExperienceRequired;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
		UDataTable* CharacterAttributesTable;

	UFUNCTION(BlueprintCallable, Category = "Level")
		void GainExperience(int32 ExpAmount);

	UFUNCTION(BlueprintCallable, Category = "Level")
		void CheckAndTriggerLevelUp();
	//Level Variables






	//Lock-on variables
	void ToggleHardLock();
	void ToggleSoftLock();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock On")
		void ToggleHardLockOnEffects();

	UFUNCTION(BlueprintImplementableEvent, Category = "Lock On")
		void ToggleSoftLockOnEffects();

	UFUNCTION(BlueprintCallable)
		void OnEnemyDestroyed(AEnemy* DestroyedEnemy);

		void ToggleHardLockOnNextEnemy();


	UFUNCTION(BlueprintCallable, Category = "Lock On")
		void TurnOffHardLock();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lock On")
		bool bIsSoftLockEnabled;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lock On")
		bool bIsHardLockEnabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On")
		float SoftLockRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On")
		float SoftLockAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lock On")
		AActor* SoftLockedEnemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lock On")
		AActor* HardLockedEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock On")
		TArray<AActor*> LockOnCandidates;

	float MaxHardLockRange;

	float HeightTargetingOffest;
		//Lock-on variables






	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	APlayerController* PlayerContr;





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
