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
#include "Technique_Struct.h"
#include "Blueprint/UserWidget.h"
#include "Command_Menu_Widget.h"
#include "Ren_Low_Poly_Character.generated.h"


class AEnemy_Poly;



UCLASS()
class PROJECT_ATLA_API ARen_Low_Poly_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARen_Low_Poly_Character();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//Movement
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	//Movement


	//Combat 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool SwordChosen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool StaffChosen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool Attacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool ActivatePlungeRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool Rolling;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void InflictDamageOnEnemy(AEnemy_Poly* Enemy);

	UFUNCTION(BlueprintCallable)
		void IncreaseStats(float AdditionalHealth, float AdditionalAttack, float AdditionalDefence);

	//UFUNCTION(BlueprintCallable, Category = "Combat")
	//	void InflictElementalDamageOnEnemy(AEnemy_Poly* Enemy);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		bool bIncreaseAbilityPoints;

	bool AnimIsPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		UAnimMontage* AbilityAnimation;

	UFUNCTION(BlueprintCallable)
		void IncreaseAbilityPoints(float Amount);

	UFUNCTION(BlueprintCallable)
		void UseAbility();

	UFUNCTION(BlueprintCallable)
		void CheckAbilityUsage();

	void DisableInputWhilePlayingAnimation();
	//Ability



	//Technique
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
		FTechnique_Struct TechniqueStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
		float GaugeIncreaseRate;

	UFUNCTION(BlueprintCallable)
		void CheckGaugeMaximum();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
		TArray<FTechnique_Struct> Techniques;

	UFUNCTION(BlueprintCallable)
		void UseTechnique(int32 TechniqueIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* DownwardSlashAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* PowerStrikeAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* FuryStrikeAnimMontage;
	//Technique


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

	UFUNCTION(BlueprintCallable)
		void IncreaseHealth(float HealAmount);

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
		void InflictAbilityDamageOnEnemy(AEnemy_Poly* Enemy, int32 TechniqueIndex);

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void CalculateTotalAttack();

	UFUNCTION(BlueprintCallable)
	void IncreaseAttack(float IncreaseAmount, float Duration);

	void RevertAttack(float IncreaseAmount);

	FTimerHandle AttackBonus;

	float OriginalAttack;
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

	float OriginalDefence;

	UFUNCTION(BlueprintCallable)
	void IncreaseDefence(float IncreaseAmount, float Duration);

	void RevertDefence(float IncreaseAmount);

	FTimerHandle DefenceBonus;
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

	//UFUNCTION(BlueprintCallable, Category = "Level")
	//	void GainExperience(int32 ExpAmount);

	//UFUNCTION(BlueprintCallable, Category = "Level")
	//	void CheckAndTriggerLevelUp();
	//Level



	//Command
	UPROPERTY()
		UCommand_Menu_Widget* CommandMenuWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> CommandMenuWidgetClass;

	UFUNCTION()
		void ToggleCommandMenu();

	UFUNCTION()
		void OpenInventory();

	UFUNCTION()
		void HandleBackInput();

	UFUNCTION()
		void SetInputModeForUI();

	UFUNCTION()
		void SetInputModeForGameplay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		bool bIsInUIMode;

	void EnableUIInputWithGameInput();
	//Command


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Character Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;
	//Character Components

private:	

	
	
};
