// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InputComponent.h"
#include "Components/ChildActorComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"
#include "Health_Struct.h"
#include "Mana_Struct.h"
#include "Ability_Struct.h"
#include "Elemental_Attack_Type.h"
#include "Elemental_Struct.h"
#include "Elemental_Proficiency_Struct.h"
#include "Engine/DataTable.h"
#include "Character_Attributes.h"
#include "Blueprint/UserWidget.h"
#include "Technique_Struct.h"
#include "Weapon_Proficiency_Struct.h"
#include "Command_Menu_Widget.h"
#include "Enemy_Detection_Arrow.h"
#include "Inventory.h"
#include "Action_Banner_Widget.h"
#include "End_Screen_Widget.h"
#include "Pause_Menu_Widget.h"
#include "Camera/PlayerCameraManager.h"  // Add this
#include "Camera_Shake_Base.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "GameEnums.h"
#include "Floating_Combat_Text_Widget.h"
#include "Demo_End_Screen_Widget.h"
#include "Ren_Low_Poly_Character.generated.h"




UENUM(BlueprintType)
enum class ESpecialPowerUp : uint8
{
	Berserk      UMETA(DisplayName = "Berserk"),
	Invulnerability UMETA(DisplayName = "Invulnerability"),
	HealthRegen     UMETA(DisplayName = "Health-Regen"),
	DoublePoints UMETA(DisplayName = "Double Points"),

	Max UMETA(Hidden)
};


USTRUCT(BlueprintType)
struct FWeaponTechniqueMap
{
	GENERATED_BODY()

		UPROPERTY()
		TMap<int32, FString> LevelToTechnique;
};




USTRUCT(BlueprintType)
struct FWeaponElementalProgression
{
	GENERATED_BODY()

	//	UPROPERTY()
	//	TMap<EElementalAttackType, FElemental_Struct> ElementalProgression;

		 // Map Level to Elemental Attack Name
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Progression")
		TMap<int32, FString> LevelToAttack;
};



USTRUCT(BlueprintType)
struct FWeaponElementalProficiencyMap
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Elemental Proficiency")
		TMap<EWeaponType, FElemental_Proficiency_Struct> ElementalWeaponProficiencyMap;
};



USTRUCT(BlueprintType)
struct FWeaponElementalAttacks
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, Category = "Elemental Attacks")
		TArray<FElemental_Struct> ElementalAttacks;
};


USTRUCT(BlueprintType)
struct FWeaponTechniques
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, Category = "Elemental Attacks")
		TArray<FTechnique_Struct> WeaponTechniques;
};



USTRUCT(BlueprintType)
struct FQueuedAction
{
	GENERATED_BODY()

		UPROPERTY()
		EQueuedActionType ActionType;

	// Data for different action types
	UPROPERTY()
		int32 TechniqueIndex;

	UPROPERTY()
		FElemental_Struct ElementalAttack;

	UPROPERTY()
		FInventoryItem ItemToUse;

	// Constructor
	FQueuedAction()
		: ActionType(EQueuedActionType::None)
		, TechniqueIndex(-1)
	{}
};



class AEnemy_Poly;
class AResults_camera;





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


	//Game mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Mode")
		class AGameMode* LowPoly_Survival_GameMode;
	//Game mode


	void DisplayWeaponStats(EWeaponType WeaponType);

	//Combat 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool SwordChosen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool StaffChosen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool Attacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool ActivatePlungeRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool Rolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bPerformingTechnique;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool isAttackedFromBehind;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float ActualDamageApplied;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void InflictDamageOnEnemy(AEnemy_Poly* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void InflictElementalDamageOnEnemy(AEnemy_Poly* Enemy, EElementalAttackType ElementType);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void UpdateStatsBasedOnWeapon();

	UFUNCTION(BlueprintCallable)
		void IncreaseStats(float AdditionalHealth, float AdditionalAttack, float AdditionalDefence, float AdditionalElemental);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float CalculatedDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float CalculatedElementalDamage;

	UFUNCTION(BlueprintImplementableEvent)
		void RemoveGameplayUI();

	UPROPERTY()
		FTimerHandle TechniqueTimerHandle;

	UFUNCTION(BlueprintCallable)
		void OnTechniqueBegin();

	UFUNCTION(BlueprintCallable)
		void OnTechniqueEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bPerformingAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bPerformingElemental;

	UPROPERTY()
		FTimerHandle ElementalTimerHandle;

	int32 InitialWeaponLevel;

	UPROPERTY()
		FTimerHandle AbilityTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float OptimalAttackDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float DistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackMovementSpeed;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void KeepInFrontOfEnemy(AActor* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void ApplyKnockbackToEnemy(AEnemy_Poly* Enemy, float KnockbackForce = 500.0f, float KnockbackDuration = 0.2f);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bEnemyIsHit;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bIsSwordTraceActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* HurtAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* BehindHurtAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bAttackedFromBehind;

	UPROPERTY(BlueprintReadWrite, Category = "Character")
		bool bIsInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
		bool bIsHurt;

	// Variables to track which part of the sequence we're in
	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		bool bIsDown = false;

	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		bool bIsGettingUp = false;

	void OnHurtAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	//Combat



	//Floating combat text UI
	UFUNCTION(BlueprintCallable, Category = "Combat|Visual")
		void SpawnFloatingCombatText(const FString& Text, const FVector& Location,
			const FLinearColor& Color = FLinearColor::White,
			bool bIsCritical = false,
			float LifeSpan = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Combat|Visual")
		void SpawnFloatingStatusText(const FString& StatusEffect, const FVector& Location);

	// Add in the class private or protected section
	UPROPERTY(EditDefaultsOnly, Category = "Combat|UI")
		TSubclassOf<UFloating_Combat_Text_Widget> FloatingCombatTextClass;
	//Floating combat text UI



	//High score
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
		int32 SwordHighScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
		int32 StaffHighScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		bool bIsGreaterThanHighScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		bool bRenderTarget;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		//UAnimMontage* VictoryAnim;



	void UpdateHighScore();

	int32 OldHighScoreValue;
	//High score



	//Item effects
	UFUNCTION(BlueprintCallable)
		void IncreaseAll();

	//Item effects




	//Save Data
	void SaveHighScore();
	void LoadHighScore();

	UFUNCTION(BlueprintCallable)
	void SavePlayerProgress();

	void LoadPlayerProgress();



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game State")
		bool bIsGameLoaded = false;
	//Save Data



	//Ability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		FAbility_Struct AbilityStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		bool bCanUseAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		bool bIncreaseAbilityPoints;

	bool AnimIsPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		UAnimMontage* AbilitySwordAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		UAnimMontage* AbilityStaffAnimation;

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

	UFUNCTION(BlueprintCallable)
		void CheckTechniquePointsMaximum();

	UFUNCTION(BlueprintCallable)
		void ControlTechniqueGaugeFill();

	UFUNCTION(BlueprintCallable)
		void UseTechnique(int32 TechniqueIndex);

	UFUNCTION(BlueprintCallable)
		void IncreaseTechniquePoints(int IncreaseAmount);

	UFUNCTION(BlueprintCallable)
		void CheckTechniquePoints();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
		bool bIsTechniquePointsMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Technique")
		TArray<FTechnique_Struct> Techniques;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanIncreaseTechniqueBarRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* VoltageBreakerAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* StormStrikeFlurryAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* TempestBarrageAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* StaticRushAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* MeteorStrikeAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* StoneRushAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* FrostRainAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Techniques")
		UAnimMontage* FeudFangAnimMontage;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Techniques")
		TArray<bool> TechniqueAvailability;
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
		void ReceiveDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
		void IncreaseHealth(float HealAmount);

	UFUNCTION(BlueprintCallable)
		void Death();

	UFUNCTION(BlueprintCallable)
		void RecoverHealth();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float DamageReduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool bIsHit;

	float PreviousMaxHealth;
	float InitialMaxHealth;
	//Health


	//Mana
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mana")
		FMana_Struct ManaStruct;


	UFUNCTION(BlueprintCallable)
		void IncreaseMana(float ManaAmount);

	void ControlMPFill();

	float PreviousMaxMana;
	float InitialMaxMana;
	//Mana


	//Attack Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float BaseAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float AttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float TotalAttack;

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void InflictTechniqueDamageOnEnemy(AEnemy_Poly* Enemy, int32 TechniqueIndex);

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void CalculateTotalAttack();

	UFUNCTION(BlueprintCallable)
	void IncreaseAttack(float IncreaseAmount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float AttackIncrease;

	float PreviousAttackPower;
	float InitialAttack;
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

	UFUNCTION(BlueprintCallable)
	void IncreaseDefence(float IncreaseAmount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defence")
		float DefenceIncrease;

	float PreviousDefense;
	float InitialDefense;
	//Defence Stats





	//Elemental Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float BaseElementalAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float ElementalMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float TotalElementalAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		bool bCastingLightningRay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FElemental_Struct ElementalStruct;

	EElementalAttackType CurrentElementalAttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TArray<FElemental_Struct> ElementalAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> FireProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> IceProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> ThunderProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* FireProjectileAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* IceProjectileAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* ThunderProjectileAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> FireAOEClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> IceAOEClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> ThunderAOEClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* FireAOEAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* IceAOEAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* ThunderAOEAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> FireGroundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> IceGroundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TSubclassOf<AActor> ThunderGroundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* FireGroundAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* IceGroundAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UAnimMontage* ThunderGroundAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		TMap<EWeaponType, FWeaponElementalProgression> WeaponElementalMap;

	// Elemental proficiency data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Proficiency")
		FElemental_Proficiency_Struct ElementalProficiency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Attacks")
		TMap<EWeaponType, FWeaponElementalAttacks> WeaponElementalAttacks;

	UFUNCTION()
		void AddElementalAttackDelayed(const FElemental_Struct& ElementalAttack, EWeaponType TheWeaponType);

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void CalculateElementalAttack();

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void UseElementalAttack(const FElemental_Struct& Attack);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Attacks")
		TMap<EElementalAttackType, FElemental_Struct> ElementalAttackMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Elemental Proficiency")
		FWeaponElementalProficiencyMap WeaponElementalProficiency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UMaterialInstance* FreezeOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UMaterialInstance* StunOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		UMaterialInstance* BurnOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		USoundBase* ThunderSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		USoundBase* IceSound;

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void SpawnElementalProjectile();

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void SpawnElementalAOE(FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void SpawnElementalGround(FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void AddExperienceToElementalProfiency(EWeaponType TheWeaponType, EElementalAttackType ElementType, float EXPAmount);

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void UnlockElementalAbility(EWeaponType TheWeaponType, EElementalAttackType ElementType);

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void ApplyTheBurnEffect(AEnemy_Poly* Enemy, float Duration, float DamagePerSecond);

	

	// Handle for the timer
	FTimerHandle BurnTimerHandle;
	FTimerHandle BurnEndTimerHandle;
	FTimerHandle Leveluphandle;
	FTimerHandle TimerHandle_UnlockElementalAbility;
	// Store the remaining burn time
	float BurnDurationRemaining;

	// Store the damage per second
	float DamagePerSecond;

	AEnemy_Poly* BurnedEnemy;


	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void ApplyFreezeEffect(AEnemy_Poly* Enemy, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Elemental")
		void ApplyStunEffect(AEnemy_Poly* Enemy, float Duration);

	void InitialiseElementalAttacks();

	void InitialiseDefaultElementalProficiencyValues();
	void InitialiseElementalProficiencies();
	void EnsureAllInitialisation();
	void VerifyInitialisation();
	void VerifyElementalAttacks();
	void LogCurrentELementalAttacks();


	float PreviousElementalPower;
	float InitialElemental;
	int32 InitialFireLevel;
	int32 InitialIceLevel;
	int32 InitialThunderLevel;
	//Elemental Stats



	//Power-Up system
	UPROPERTY(BlueprintReadWrite, Category = "Power-Ups")
		ESpecialPowerUp CurrentPowerUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power-Ups")
		UAnimMontage* PowerUpAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power-Ups")
		UChildActorComponent* PowerUpCamera;

	UPROPERTY(BlueprintReadWrite, Category = "Power-Ups")
		bool bPowerUpActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power-Ups")
		bool bIsPoweringUp;

	// Function to apply the selected power-up
	UFUNCTION(BlueprintCallable, Category = "Power-Ups")
		void ApplyPowerUp(ESpecialPowerUp PowerUp);

	UFUNCTION(BlueprintImplementableEvent, Category = "Power-Ups")
		void ActivatePowerUpVFX();

	UFUNCTION(BlueprintCallable, Category = "Power-Ups")
		void ResetAttackPower();

	UFUNCTION(BlueprintCallable, Category = "Power-Ups")
		void NullifyInvulnerability();

	UFUNCTION(BlueprintCallable, Category = "Power-Ups")
		void RegenHealth();

	UFUNCTION(BlueprintCallable, Category = "Power-Ups")
		void CancelHealthRegen();

	UFUNCTION(BlueprintCallable, Category = "Power-Ups")
		void CancelDoublePoints();

	UFUNCTION(BlueprintCallable, Category = "Power-Ups")
		void DecreaseHealth(int amount);

	bool bPowerUpPending;

	FTimerHandle ResetAttackTimer;
	FTimerHandle InvulnerabilityTimer;
	FTimerHandle RegenHealthTimer;
	FTimerHandle RegenHealthDurationTimer;
	FTimerHandle DoublePointsTimer;
	FTimerHandle ElementalAttackInitTimerHandle;
	//Power-Up system



	//Level & Weapon Proficiency
	// Map to store proficiency for each weapon type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
		TMap<EWeaponType, FWeapon_Proficiency_Struct> WeaponProficiencyMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Proficiency")
		FWeapon_Proficiency_Struct Weapon_Proficiency_Struct;

	UPROPERTY()
		TMap<EWeaponType, FWeaponTechniqueMap> WeaponLevelToTechniqueMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Technique System")
		TArray<FString> QueuedUnlockTechniques;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Technique System")
		bool bQueuedLevelUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Attacks")
		TMap<EWeaponType, FWeaponTechniques> WeaponTechniques;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental Attacks")
		bool bLevelUp;

	void GenerateStatUpgradeMessages();

	// Function to add EXP to the current weapon (called when an enemy is defeated)
	UFUNCTION(BlueprintCallable)
		void AddWeaponEXP(float ExpAmount);

	UFUNCTION(BlueprintImplementableEvent)
		void ActivateLevelUpVFX();

	UFUNCTION(BlueprintCallable, Category = "Weapon Proficiency")
		void CheckAndApplyWeaponLevelUp(EWeaponType TheWeaponType);

	UFUNCTION(BlueprintCallable, Category = "Weapon Techniques")
		void InitializeWeaponTechniques();

	UFUNCTION(BlueprintCallable, Category = "Weapon Techniques")
		void UnlockWeaponTechnique(EWeaponType TheWeaponType, int32 CurrentLevel);

	TArray<float> QueuedEXP;

	//Level & Weapon Proficiency



	//Camera Shake
	UFUNCTION(BlueprintCallable)
		void TriggerCameraShake();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		TSubclassOf<UCameraShakeBase> HitCameraShake;
	//Camera Shake



	//Command
	UPROPERTY()
		UCommand_Menu_Widget* CommandMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> CommandMenuWidgetClass;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		UUserWidget* InventoryButtonWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		bool bIsInventoryEmpty;

	void SetItemsButtonFocus();

	UFUNCTION()
		void ToggleCommandMenu();

	UFUNCTION()
		void OpenInventory();

	UFUNCTION()
		void OpenTechniques();

	UFUNCTION()
		void OpenElementalAttacks();

	UFUNCTION()
		void HandleBackInput();

	UFUNCTION()
		void SetInputModeForUI();

	UFUNCTION()
		void SetInputModeForGameplay();





	void EnableUIInputWithGameInput();

	void UpdateVisibilityBasedOnIndex(int Index);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		bool bIsInUIMode;

	// Track if the inventory is open
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		bool bIsInventoryOpen = false;

	// Track if the Techniques is open
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		bool bIsTechniquesOpen = false;

	// Track if the Techniques is open
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		bool bIsElementalsOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundBase* BackSound;

	UButton* LastFocusedButton;

	const float CommandModeTimeDilation = 0.05f;

	bool bIsInCommandMode = false;

	// Command mode functions
	UFUNCTION(BlueprintCallable, Category = "Command System")
		void EnterCommandMode();

	UFUNCTION(BlueprintCallable, Category = "Command System")
		void ExitCommandMode();

	UFUNCTION(BlueprintCallable, Category = "Command System")
		bool IsInCommandMode() const { return bIsInCommandMode; }

	UFUNCTION(BlueprintCallable, Category = "Command System")
		void EnablePPV();

	UFUNCTION(BlueprintCallable, Category = "Command System")
		void DisablePPV();

	void UpdateUIInCommandMode();

	// Post Process Volume reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command Menu")
		class APostProcessVolume* CommandMenuPPV;

	// Timeline for smooth transitions
	UPROPERTY()
		FTimeline PPVTimeline;

	// Curve to control the transition
	UPROPERTY(EditAnywhere, Category = "Command Menu")
		class UCurveFloat* PPVCurve;

	void UpdatePPVWeight(float Value);
	void OnPPVTimelineFinished();

	UPROPERTY()
		float CurrentPPVWeight = 0.0f;

	UPROPERTY()
		float TargetPPVWeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Command Menu")
		float PPVTransitionSpeed;

	FTimerHandle UIUpdateTimerHandle;
	//Command


	//Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class UInventory* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FInventoryItem CurrentItemBeingUsed;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UNotification_Widget> NotificationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		UNotification_Widget* NotificationWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		UAnimMontage* ItemUseAnimaiton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bUsingItem;

	// Function to create notification widget
	void CreateNotificationWidget();
	//Inventory


	//Scoring system
	UFUNCTION(BlueprintCallable, Category = "Score")
		void AddPoints(int32 Points);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
		int32 PlayerScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
		bool bDoublePoints;
	//Scoring system


	//Action Banner
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UAction_Banner_Widget> ActionBannerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* ActionBannerWidgetComponent;

	UFUNCTION()
		void SpawnActionBanner(const FString& Text);
	//Action Banner


	//Enemy Arrow UI
	UPROPERTY()
		TArray<UEnemy_Detection_Arrow*> EnemyArrowWidgets;

	// Class reference for the Enemy Arrow Widget
	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> EnemyArrowWidgetClass;
	//Enemy Arrow UI

	void UpdateEnemyArrows();
	void AddEnemyArrow(AEnemy_Poly* Enemy);
	void OnEnemyDestroyed(AActor* DestroyedActor);
	void RemoveAllEnemyArrows();

	// Helper function to check if enemy is off-screen and update the arrow
	void CheckAndDisplayArrow(AActor* Enemy, UEnemy_Detection_Arrow* ArrowWidget);

	TMap<AEnemy_Poly*, UEnemy_Detection_Arrow*> EnemyArrowMap;
	//Enemy Arrow UI



	//Game over
	UPROPERTY()
		UEnd_Screen_Widget* EndScreenWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> EndScreenWidgetClass;

	UPROPERTY()
		class AResults_camera* Results_Camera;

	void FindResultsCamera();

	void DisplayEndScreenWidget();
	//Game over



	//Pause Menu
	 // Pause menu widget class reference
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UPause_Menu_Widget> PauseMenuClass;

	// Pause menu widget instance
	UPROPERTY()
		UPause_Menu_Widget* PauseMenuWidget;

	// Function to handle pause input
	UFUNCTION()
		void HandlePauseGame();

	// Helper function to set game pause state
	void SetGamePaused(bool bPaused);

	UFUNCTION(BlueprintPure, Category = "Pause")
		bool IsGamePaused() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundBase* PauseSound;
	//Pause Menu


	//Force Feedback
	UFUNCTION(BlueprintCallable, Category = "Feedback")
		void TriggerVibration(float Intensity, float Duration, bool bLeftLarge = true, bool bRightLarge = true);
	//Force Feedback


	//CamShake
	UPROPERTY(EditAnywhere, Category = "Camera")
		TSubclassOf<UCamera_Shake_Base> MyCameraShakeClass;


	// Expose this for animation blueprint
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void SetInvulnerabilityState(bool bInvulnerable);

	// Flags for combat state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	bool bIsInCombatAction = false;    // True during attacks, dodges, etc

	bool bCanAccessMenus;       // For menu access
	bool bIsInvulnerable = false;      // True during iframe windows

	// Function to handle specific input blocks
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CanPerformCombatAction() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		bool CanAccessMenus() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCombatActionState(bool bInCombatAction);
	// Flags for combat state



	//Action queueing system
	// Add these to your character class public section
	UPROPERTY()
		TArray<FQueuedAction> ActionQueue;

	UFUNCTION(BlueprintCallable)
		void ProcessNextAction();

	UFUNCTION()
		bool IsPlayingAnyAction() const;

	UFUNCTION()
		void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
		void ClearCurrentAction();

	UFUNCTION()
		bool CanInterruptCurrentAction(EQueuedActionType NewAction);

	void InterruptCurrentAnimation();


	UFUNCTION(BlueprintCallable, Category = "Combat")
		bool CanRoll() const;

	void ForceResetTechniqueFlags();


	// Debug function to force reset all combat states
	UFUNCTION(BlueprintCallable, Category = "Debug")
		void ForceResetAllCombatStates();

	// Helper to check combat states - use for debugging
	UFUNCTION(BlueprintCallable, Category = "Debug")
		void LogCombatStates(FString Context);
	//Action queueing system



	//Objective Message
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power-Ups")
		UChildActorComponent* ObjectiveCamera;
	//Objective Message

	bool bObjectiveSequencePlaying = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Character Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
		USceneComponent* StaffFireProjectile;
	//Character Components

	//Demo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Demo")
	bool bIsDemoBuild = false;

	static constexpr int32 DEMO_MAX_LEVEL = 5;

	void DisplayDemoScreen();

	UPROPERTY()
	UDemo_End_Screen_Widget* Demo_End_Screen_Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> DemoEndScreenWidgetClass;
	//Demo




private:

	// Camera zoom for command menu
	float DefaultArmLength;        // Store the original spring arm length
	float CommandModeArmLength;    // Target length when in command mode
	float CurrentArmLength;        // Current interpolated length
	float ArmLengthTransitionSpeed; // How fast to zoom in/out
	bool bCameraZoomInitialized;   // Flag to ensure we initialize zoom values
	TArray<FTimerHandle> ActiveTimerHandles;
	void PreAttackCalculation();
	float GetTargetHeightOffset(AActor* Enemy);

};
