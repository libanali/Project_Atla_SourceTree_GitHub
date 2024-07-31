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
#include "Health_Struct.h"
#include "Ability_Struct.h"
#include "Engine/DataTable.h"
#include "Character_Attributes.h"
#include "Inventory_Component.h"
#include "Item_Class.h"
#include "WorldItem.h"
#include "Ren_Low_Poly_Character.generated.h"


class AEnemy_Poly;



UCLASS()
class PROJECT_ATLA_API ARen_Low_Poly_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARen_Low_Poly_Character();



	//Movement
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	//Movement


	//Combat 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool SwordChosen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool CrossbowChosen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool StaffChosen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool Attacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool Rolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bInvincible;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void InflictDamageOnEnemy(AEnemy_Poly* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void InflictElementalDamageOnEnemy(AEnemy_Poly* Enemy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float CalculatedDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float CalculatedElementalDamage;
	//Combat



	//Ability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		FAbility_Struct AbilityStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		bool bCanUseAbility;

	bool AnimIsPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		UAnimMontage* AbilityAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		UMaterial* AbilityMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		UMaterial* OriginalMaterial;

	UFUNCTION(BlueprintCallable)
		void IncreaseAbilityPoints(float Amount);

	UFUNCTION(BlueprintCallable)
		void UseAbility();

	UFUNCTION(BlueprintCallable)
		void CheckAbilityUsage();

	void DisableInputWhilePlayingAnimation();
	//Ability



	//Lock-On variables
	void ToggleSoftLock();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lock On")
		bool bIsSoftLockEnabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On")
		float SoftLockRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lock On")
		float SoftLockAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lock On")
		AActor* SoftLockedEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock On")
		TArray<AActor*> LockOnCandidates;

	float HeightTargetingOffest;
	//Lock-On variables



	//Health
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		FHealth_Struct HealthStruct;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void TakeDamage(float DamageAmount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float DamageReduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool bIsHit;
	//Health



	//Attack Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float BaseAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float AttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float TotalAttack;

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void CalculateTotalAttack();
	//Attack Stats



	//Defence Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float BaseDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float DefenceMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float TotalDefence;

	UFUNCTION(BlueprintCallable, Category = "Defence")
		void CalculateTotalDefence();
	//Defence Stats



	//Elemental Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float BaseElementalAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float ElementalMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float TotalElementalAttack;

	UFUNCTION(BlueprintCallable, Category = "Defence")
		void CalculateElementalAttack();
	//Elemental Stats



	//Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		int32 CharacterLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		int32 ExperiencePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		TArray<int32> ExperienceRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		UDataTable* CharacterAttributesTable;

	UFUNCTION(BlueprintCallable, Category = "Level")
		void GainExperience(int32 ExpAmount);

	UFUNCTION(BlueprintCallable, Category = "Level")
		void CheckAndTriggerLevelUp();
	//Level


	//Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		UInventory_Component* InventoryComponent;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void PickupItem(AWorldItem* WorldItem);
	//Inventory

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Character Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;
	//Character Components

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
