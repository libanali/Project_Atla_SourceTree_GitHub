// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Low_Poly_Character.h"
#include "LowPoly_Survival_GameMode.h"
#include "Kismet/Gameplaystatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy_Poly.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "Command_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Camera/CameraShakeBase.h"
#include "Technique_Struct.h"
#include "Enemy_Detection_Arrow.h"
#include "Player_Save_Game.h"
#include "Game_Instance.h"
#include "Game_Over_Widget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Elemental_Attacks_List_Widget.h"

// Sets default values
ARen_Low_Poly_Character::ARen_Low_Poly_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetCharacterMovement()->AirControl = 0.1f;


	//Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;

	//Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	//Staff Projectile Point
	StaffFireProjectile = CreateDefaultSubobject<USceneComponent>(TEXT("Staff Fire Projectile"));

	//Camera child actor
	PowerUpCamera = CreateDefaultSubobject<UChildActorComponent>(TEXT("Power Up Camera"));
	PowerUpCamera->SetupAttachment(GetMesh());
	//PowerUpCamera->SetupAttachment(GetMesh(), TEXT("spine_01")); // Use "HeadSocket" if your mesh has one

	PowerUpCamera->SetRelativeLocation(FVector(0.f, 50.f, 150.f)); // Position above the character's head
	PowerUpCamera->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f)); // Tilt slightly down if needed

	//Ability
	bCanUseAbility = false;
	bIncreaseAbilityPoints = true;
	ActivatePlungeRadius = false;


	//Technique
	GaugeIncreaseRate = 10.7f;
	CanIncreaseTechniqueBarRate = false;
	bIsTechniquePointsMax = false;

	//Combat
	isAttackedFromBehind = false;


	//Score
	PlayerScore = 0;
	bDoublePoints = false;

	//High score
	SwordHighScore = 0;
	StaffHighScore = 0;
	bRenderTarget = false;



	//Lock-On
	bIsSoftLockEnabled = false;
	SoftLockRange = 2300.0f;
	SoftLockAngle = 340.0f;
	HeightTargetingOffest = 10.0f;



	//Health
	bIsDead = false;


	//Attack
	BaseAttack = 3.0f;
	AttackMultiplier = 1.2f; 
	AttackIncrease = 3.0f;


	//Defence
	BaseDefence = 300.0f;
	DefenceMultiplier = 2.0f;
	DefenceIncrease = 2.0f;


	//Elemental
	BaseElementalAttack = 4.0f;
	ElementalMultiplier = 2.0f;
	bCastingLightningRay = false;


	//Command
	bIsInUIMode = false;
	bIsInventoryOpen = false;
	bIsTechniquesOpen = false;
	bIsInventoryEmpty = true;

	//Special Power-Up
	bPowerUpActive = false;





	//WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Sword, FElemental_Proficiency_Struct());
	//WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Staff, FElemental_Proficiency_Struct());


	

	InitialiseDefaultElementalProficiencyValues();

	FDelegateHandle DuplicateCheckTimerHandle;

}







void ARen_Low_Poly_Character::MoveForward(float Axis)
{

	if (!bIsDead)

	{

		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Axis);

	}
}




void ARen_Low_Poly_Character::MoveRight(float Axis)
{

	if (!bIsDead)

	{


		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Axis);

	}

}




void ARen_Low_Poly_Character::DisplayWeaponStats(EWeaponType TheWeaponType)
{

	float Attack = 0.f;
	float Defense = 0.f;
	float ElementalAttack = 0.f;

	if (TheWeaponType == EWeaponType::Sword)
	{
		Attack = BaseAttack;
		Defense = BaseDefence;
		ElementalAttack = BaseElementalAttack;
	}
	else if (TheWeaponType == EWeaponType::Staff)
	{
		Attack = BaseAttack;
		Defense = BaseDefence;
		ElementalAttack = BaseElementalAttack;
	}

	if (WeaponProficiencyMap.Contains(TheWeaponType))
	{
		const FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[TheWeaponType];
		Attack += Proficiency.AttackPowerBoost;
		Defense += Proficiency.DefenseBoost;
		ElementalAttack += Proficiency.ElementalPowerBoost;
	}

	// Log the final stats
	UE_LOG(LogTemp, Warning, TEXT("Weapon Type: %d, Attack: %.2f, Defense: %.2f, Elemental Attack: %.2f"),
		(int32)TheWeaponType, Attack, Defense, ElementalAttack);

}



void ARen_Low_Poly_Character::InflictDamageOnEnemy(AEnemy_Poly* Enemy)
{

	if (Enemy)

	{

		float AttackDamage = BaseAttack;
		float EffectiveDefence = FMath::Clamp(Enemy->DefencePercentage / 100.0f, 0.0f, 1.0f);
		CalculatedDamage = AttackDamage * (1 - EffectiveDefence);


		UWorld* World = GetWorld();
		if (World)

		{

			float ActualDamageApplied = Enemy->ApplyDamage(CalculatedDamage, FHitResult(), GetController(), this);

		}


	}


}



void ARen_Low_Poly_Character::InflictElementalDamageOnEnemy(AEnemy_Poly* Enemy, EElementalAttackType ElementType)
{

	if (Enemy)
	{
		// Default damage is the base elemental attack
		float ElementalDamage = BaseElementalAttack;

		// Find the elemental attack type in the ElementalAttacks array
		for (const FElemental_Struct& ElementalAttack : ElementalAttacks)
		{
			if (ElementalAttack.ElementalType == ElementType)
			{
				ElementalDamage *= ElementalAttack.DamageMultiplier; // Apply damage multiplier
				break;
			}
		}


		float EffectiveDefence = FMath::Clamp(Enemy->DefencePercentage / 100.0, 0.0f, 1.0f);
		float TheCalculatedDamage = ElementalDamage * (1 - EffectiveDefence);

		// Apply the calculated damage to the enemy
		UWorld* World = GetWorld();
		if (World)
		{
			float ActualDamageApplied = Enemy->ApplyDamage(TheCalculatedDamage, FHitResult(), GetController(), this);

			// Optional: Add debug logging
			UE_LOG(LogTemp, Log, TEXT("Inflicted %f %s damage to %s."), TheCalculatedDamage, *UEnum::GetValueAsString(ElementType), *Enemy->GetName());
		}
	}

}




void ARen_Low_Poly_Character::UpdateStatsBasedOnWeapon()
{

	// Reset to base stats
	if (WeaponType == EWeaponType::Sword)
	{
		BaseAttack = 3.0f;
		BaseDefence = 2.0f;
		BaseElementalAttack = 4.0f;
		HealthStruct.MaxHealth = 140.0f;
		ManaStruct.MaxMana = 60.0f;
	}
	else if (WeaponType == EWeaponType::Staff)
	{
		BaseAttack = 20.0f;
		BaseDefence = 2.0f;
		BaseElementalAttack = 10.0f;
		HealthStruct.MaxHealth = 130.0f;
		ManaStruct.MaxMana = 95.0f;
	}

	// Apply proficiency boosts
	if (WeaponProficiencyMap.Contains(WeaponType))
	{
		const FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[WeaponType];
		BaseAttack += Proficiency.AttackPowerBoost;
		BaseDefence += Proficiency.DefenseBoost;
		BaseElementalAttack += Proficiency.ElementalPowerBoost;
		HealthStruct.MaxHealth += Proficiency.MaxHealthBoost;
		ManaStruct.MaxMana += Proficiency.MaxManaBoost;
	}

	
}





void ARen_Low_Poly_Character::IncreaseStats(float AdditionalHealth, float AdditionalAttack, float AdditionalDefence, float AdditionalElemental)
{

	float HealthAdditionalAmount = HealthStruct.MaxHealth *= AdditionalHealth;
	float AttackAdditionalAmount = BaseAttack *= AdditionalAttack;
	float DefenceAdditionalAmount = BaseDefence *= AdditionalDefence;
	float ElementalAdditionalAmount = BaseElementalAttack += AdditionalElemental;

	HealthStruct.CurrentHealth = HealthStruct.MaxHealth;


}




void ARen_Low_Poly_Character::SpawnPlayerCharacterForRender()
{
	bRenderTarget = true;

	if (bRenderTarget && bIsDead)

	{

		FVector RenderSpawnLocation = FVector(-3249.0, 570.0, 207.0);

		SetActorLocation(RenderSpawnLocation);

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Character Spawned to new location"));

	}


}






void ARen_Low_Poly_Character::UpdateHighScore(int32 NewScore)
{
	// Check which weapon type the player is using and compare the score
	if (WeaponType == EWeaponType::Sword)
	{
		if (NewScore > SwordHighScore)
		{
			SwordHighScore = NewScore;
			UE_LOG(LogTemp, Warning, TEXT("New Sword High Score: %d"), SwordHighScore);
		}
	}
	else if (WeaponType == EWeaponType::Staff)
	{
		if (NewScore > StaffHighScore)
		{
			StaffHighScore = NewScore;
			UE_LOG(LogTemp, Warning, TEXT("New Staff High Score: %d"), StaffHighScore);
		}
	}

	// After updating the high score, save it
	SaveHighScore();
}




void ARen_Low_Poly_Character::DisplayGameOverUI()
{
	// Check if the GameOverWidgetInstance already exists, if not create it
	if (!GameOverWidgetInstance)
	{
		GameOverWidgetInstance = CreateWidget<UGame_Over_Widget>(GetWorld(), GameOverWidgetClass);
	}

	if (GameOverWidgetInstance)
	{
		// Use the player's score and high score from the character class
		int32 FinalScore = PlayerScore;

		// Determine the high score based on the equipped weapon
		int32 HighScore = (WeaponType == EWeaponType::Sword) ? SwordHighScore : StaffHighScore;

		// Set up the widget with final score and high score
		GameOverWidgetInstance->SetUpGameOverUI(FinalScore, HighScore);



		// Trigger the score animation
		//GameOverWidgetInstance->StartScoreAnimation();
		if (Results_Camera)
		{

			GameOverWidgetInstance->Results_Camera = Results_Camera;

		}

		else

		{

			UE_LOG(LogTemp, Error, TEXT("Results_Camera reference is null!"));

		}
		// Add the widget to the viewport if it's not already there
		if (!GameOverWidgetInstance->IsInViewport())
		{
			GameOverWidgetInstance->AddToViewport();
		}

		// Update the high score if the final score exceeds it
	//	UpdateHighScore(FinalScore);

		SaveHighScore();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Game Over Widget!"));
	}
}



void ARen_Low_Poly_Character::AddPoints(int32 Points)
{

	if (bDoublePoints)

	{
		Points *= 2;
	}

	PlayerScore += Points;

}




void ARen_Low_Poly_Character::Score_Reaction_Anim()
{

	// Ensure WeaponType is set correctly
	if (WeaponType == EWeaponType::Sword)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using Sword Weapon"));
	}
	else if (WeaponType == EWeaponType::Staff)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using Staff Weapon"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unknown Weapon Type!"));
	}

	// Fetch the correct high score based on WeaponType
	int32 HighScore = (WeaponType == EWeaponType::Sword) ? SwordHighScore : StaffHighScore;

	// Debug log to verify HighScore selection
	UE_LOG(LogTemp, Warning, TEXT("HighScore selected: %d"), HighScore);

	if (GameOverWidgetInstance)
	{
		if (GameOverWidgetInstance->CurrentDisplayedScore > HighScore)
		{
			//PlayAnimMontage(VictoryAnim, 1.0f); // Cheer animation
			UE_LOG(LogTemp, Warning, TEXT("New High Score!"));
			bIsGreaterThanHighScore = true;
		}
		else
		{

			UE_LOG(LogTemp, Warning, TEXT("Try Harder..."));
			bIsGreaterThanHighScore = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverWidgetInstance is null!"));
	}

}



void ARen_Low_Poly_Character::FindResultsCamera()
{

	TArray<AActor*> TaggedActors;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("results_camera"), TaggedActors);

	if (TaggedActors.Num() > 0)
	{
		Results_Camera = TaggedActors[0]; // Get the first actor with the tag
		UE_LOG(LogTemp, Log, TEXT("Results Camera found: %s"), *Results_Camera->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No actor found with the tag 'results_camera'!"));
	}


}




void ARen_Low_Poly_Character::IncreaseAll()
{

	HealthStruct.CurrentHealth = HealthStruct.MaxHealth;
	ManaStruct.CurrentMana = ManaStruct.MaxMana;
	TechniqueStruct.TechniquePoints = TechniqueStruct.MaxTechniquePoints;

}





void ARen_Low_Poly_Character::SaveHighScore()
{

	UPlayer_Save_Game* SaveGameInstance = Cast<UPlayer_Save_Game>(UGameplayStatics::CreateSaveGameObject(UPlayer_Save_Game::StaticClass()));

	// Check if the SaveGameInstance was created successfully
	if (SaveGameInstance)
	{
		// Set the high score data (Sword and Staff scores)
		SaveGameInstance->SwordHighScore = SwordHighScore;
		SaveGameInstance->StaffHighScore = StaffHighScore;
		SaveGameInstance->SavedWeaponProficiencyMap = WeaponProficiencyMap;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player Save Slot"), 0);
	}

}



void ARen_Low_Poly_Character::LoadHighScore()
{

	UPlayer_Save_Game* LoadGameInstance = Cast<UPlayer_Save_Game>(UGameplayStatics::LoadGameFromSlot(TEXT("Player Save Slot"), 0));

	if (LoadGameInstance)

	{

		SwordHighScore = LoadGameInstance->SwordHighScore;
		StaffHighScore = LoadGameInstance->StaffHighScore;
		WeaponProficiencyMap = LoadGameInstance->SavedWeaponProficiencyMap;

		UE_LOG(LogTemp, Warning, TEXT("Loaded Sword High Score: %d"), SwordHighScore);
		UE_LOG(LogTemp, Warning, TEXT("Loaded Staff High Score: %d"), StaffHighScore);

		// Debug log for testing
		for (const TPair<EWeaponType, FWeapon_Proficiency_Struct>& Pair : WeaponProficiencyMap)
		{
			const EWeaponType TheWeaponType = Pair.Key;
			const FWeapon_Proficiency_Struct& Proficiency = Pair.Value;
			UE_LOG(LogTemp, Log, TEXT("Loaded %s: Level %d, EXP %.2f"),
				*UEnum::GetValueAsString(TheWeaponType),
				Proficiency.WeaponLevel,
				Proficiency.CurrentEXP);
		}

	}

	else

	{

		SwordHighScore = 0;
		StaffHighScore = 0;

		UE_LOG(LogTemp, Warning, TEXT("No saved data found. Resetting high scores."));
		UE_LOG(LogTemp, Warning, TEXT("No save game found, using default proficiency values."));

	}

}



void ARen_Low_Poly_Character::SavePlayerProgress()
{

	UPlayer_Save_Game* SaveGameInstance = Cast<UPlayer_Save_Game>(UGameplayStatics::CreateSaveGameObject(UPlayer_Save_Game::StaticClass()));
	if (SaveGameInstance)
	{

		FWeaponTechniques CurrentWeaponTechniques;
		CurrentWeaponTechniques.WeaponTechniques = Techniques;
		WeaponTechniques.Add(WeaponType, CurrentWeaponTechniques);

		// Copy data to save instance
		SaveGameInstance->SavedWeaponProficiencyMap = WeaponProficiencyMap;
		SaveGameInstance->SavedElementalProficiencyMap = WeaponElementalProficiency.ElementalWeaponProficiencyMap;
		SaveGameInstance->SavedWeaponElementalAttacks = WeaponElementalAttacks;
		SaveGameInstance->SavedWeaponTechniques = WeaponTechniques;

		// EXTENSIVE DEBUGGING
		UE_LOG(LogTemp, Error, TEXT("=== SAVING GAME - DETAILED ATTACK DUMP ==="));
		for (const auto& WeaponPair : SaveGameInstance->SavedWeaponElementalAttacks)
		{
			EWeaponType TheWeaponType = WeaponPair.Key;
			const FWeaponElementalAttacks& Attacks = WeaponPair.Value;

			UE_LOG(LogTemp, Error, TEXT("Weapon Type: %s"), *UEnum::GetValueAsString(TheWeaponType));
			UE_LOG(LogTemp, Error, TEXT("Total Attacks: %d"), Attacks.ElementalAttacks.Num());

			for (const FElemental_Struct& Attack : Attacks.ElementalAttacks)
			{
				UE_LOG(LogTemp, Error, TEXT("  Attack Name: %s"), *Attack.ElementalAttackName);
				UE_LOG(LogTemp, Error, TEXT("    Level: %d"), Attack.ElementalLevel);
				UE_LOG(LogTemp, Error, TEXT("    Type: %s"), *UEnum::GetValueAsString(Attack.ElementalType));
				UE_LOG(LogTemp, Error, TEXT("    Is Unlocked: %s"), Attack.bIsUnlocked ? TEXT("TRUE") : TEXT("FALSE"));
			}
		}
		UE_LOG(LogTemp, Error, TEXT("=== END OF SAVING GAME DUMP ==="));

		// Save to slot
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player Save Slot"), 0))
		{
			UE_LOG(LogTemp, Log, TEXT("Player progress saved successfully."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to save player progress."));
		}
	}
}


void ARen_Low_Poly_Character::LoadPlayerProgress()
{

	bIsGameLoaded = true;

	UPlayer_Save_Game* LoadGameInstance = Cast<UPlayer_Save_Game>(
		UGameplayStatics::LoadGameFromSlot(TEXT("Player Save Slot"), 0));

	if (LoadGameInstance)
	{
		// Load the saved data
		WeaponProficiencyMap = LoadGameInstance->SavedWeaponProficiencyMap;
		WeaponElementalProficiency.ElementalWeaponProficiencyMap =
			LoadGameInstance->SavedElementalProficiencyMap;
		WeaponElementalAttacks = LoadGameInstance->SavedWeaponElementalAttacks;
		WeaponTechniques = LoadGameInstance->SavedWeaponTechniques;

		// After loading, sync the Techniques array with the loaded data
		if (WeaponTechniques.Contains(WeaponType))
		{
			Techniques = WeaponTechniques[WeaponType].WeaponTechniques;
		}
		// Rest of your existing load code...
		bIsGameLoaded = false;
	}
}




void ARen_Low_Poly_Character::Death()
{

	if (bIsDead)  // Exit early if already dead
	{
		return;
	}

	bIsDead = true;  // Mark the player as dead

	// Disable player input and overlap events only once
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		DisableInput(PlayerController);

	}

	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	// Perform any additional death-related tasks, such as saving scores and showing the UI
	SaveHighScore();
	SavePlayerProgress();

	RemoveGameplayUI();
	// Display Game Over UI only once
	DisplayGameOverUI();


}





void ARen_Low_Poly_Character::RecoverHealth()
{

	HealthStruct.CurrentHealth = HealthStruct.MaxHealth;

}





void ARen_Low_Poly_Character::IncreaseMana(float ManaAmount)
{

	ManaStruct.IncreaseMana(ManaAmount);

}



void ARen_Low_Poly_Character::ControlMPFill()
{


	bool bManaFull = ManaStruct.CurrentMana == ManaStruct.MaxMana;



	if (!bIsDead && !bManaFull)

	{

		float Delta = GetWorld()->DeltaTimeSeconds;

		ManaStruct.CurrentMana += 1.3f * Delta;

		ManaStruct.CurrentMana = FMath::Min(ManaStruct.CurrentMana, ManaStruct.MaxMana);
	}


}







void ARen_Low_Poly_Character::InflictTechniqueDamageOnEnemy(AEnemy_Poly* Enemy, int32 TechniqueIndex)
{

	if (Enemy)
	{
		// Check if the technique index is valid and the technique is unlocked
		if (TechniqueIndex >= 0 && TechniqueIndex < Techniques.Num() && Techniques[TechniqueIndex].bIsUnlocked)
		{
			// Get the base damage and technique bonus
			float BaseDamage = BaseAttack;
			float EffectiveDefence = FMath::Clamp(Enemy->DefencePercentage / 100.0f, 0.0f, 1.0f);
			float TechniqueDamageBonus = Techniques[TechniqueIndex].DamageBonus;

			// Apply the technique's damage bonus
			float FinalDamage = BaseDamage * TechniqueDamageBonus;

			// Adjust for the enemy's defense
			FinalDamage *= (1 - EffectiveDefence);

			// Apply the damage to the enemy
			UWorld* World = GetWorld();
			if (World)
			{
				float ActualDamageApplied = Enemy->ApplyDamage(FinalDamage, FHitResult(), GetController(), this);

				// Additional logic for technique effects can go here
				UE_LOG(LogTemp, Log, TEXT("Technique '%s' used, inflicting %f damage"), *Techniques[TechniqueIndex].TechniqueName, FinalDamage);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Technique is locked or does not exist!"));
		}
	}


}




void ARen_Low_Poly_Character::IncreaseAbilityPoints(float Amount)
{

	if (bIncreaseAbilityPoints)
	{

		AbilityStruct.CurrentAbilityPoints = FMath::Min(AbilityStruct.CurrentAbilityPoints + Amount, AbilityStruct.MaxAbilityPoints);

	}

	else

	{

		AbilityStruct.CurrentAbilityPoints = FMath::Min(AbilityStruct.CurrentAbilityPoints + 0, AbilityStruct.MaxAbilityPoints);


	}
}



void ARen_Low_Poly_Character::UseAbility()
{

	if (bCanUseAbility && !bIsDead)

	{

		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("Ability used!"));
		AbilityStruct.CurrentAbilityPoints = 0.0f;
		bCanUseAbility = false;


		if (WeaponType == EWeaponType::Sword)

		{

			PlayAnimMontage(AbilitySwordAnimation, 1.0f);

		}

		else if (WeaponType == EWeaponType::Staff)

		{

				PlayAnimMontage(AbilityStaffAnimation, 1.0f);

		}

	}

}



void ARen_Low_Poly_Character::DisableInputWhilePlayingAnimation()
{

	APlayerController* PlayerContr = GetWorld()->GetFirstPlayerController();

	if (AnimIsPlaying)

	{
		PlayerContr->DisableInput(PlayerContr);

	}


}




void ARen_Low_Poly_Character::CheckAbilityUsage()
{

	
	if (AbilityStruct.CurrentAbilityPoints == AbilityStruct.MaxAbilityPoints)

	{

		bCanUseAbility = true;

	}


}




void ARen_Low_Poly_Character::CheckGaugeMaximum()
{

	//Check if current gauge has enough to increase skill point.

	if (TechniqueStruct.CurrentGauge >= TechniqueStruct.MaxGauge)

	{

		TechniqueStruct.TechniquePoints++;

		if (TechniqueStruct.TechniquePoints < TechniqueStruct.MaxTechniquePoints)
		{
			TechniqueStruct.CurrentGauge = 0.0f;
		}
	}


}



void ARen_Low_Poly_Character::CheckTechniquePointsMaximum()
{

	if (TechniqueStruct.TechniquePoints == TechniqueStruct.MaxTechniquePoints)

	{

		bIsTechniquePointsMax = true;
		//TechniqueStruct.CurrentGauge = 99.0f;

		TechniqueStruct.CurrentGauge = TechniqueStruct.MaxGauge;
	}

	else

	{

		bIsTechniquePointsMax = false;


	}

}



void ARen_Low_Poly_Character::ControlTechniqueGaugeFill()
{
	if (!bIsTechniquePointsMax)
	{
		float Delta = GetWorld()->GetDeltaSeconds();

		// Increment the gauge based on Delta
		TechniqueStruct.CurrentGauge += GaugeIncreaseRate * Delta;



		// Check if we have reached the max technique points
		if (TechniqueStruct.TechniquePoints >= TechniqueStruct.MaxTechniquePoints)
		{
			TechniqueStruct.TechniquePoints = TechniqueStruct.MaxTechniquePoints; // Clamp to max
			bIsTechniquePointsMax = true; // Set max flag
			TechniqueStruct.CurrentGauge = TechniqueStruct.MaxGauge; // Keep the gauge full
		}

	}



}




void ARen_Low_Poly_Character::UseTechnique(int32 TechniqueIndex)
{
	if (TechniqueIndex >= 0 && TechniqueIndex < Techniques.Num())
	{
		FTechnique_Struct& SelectedTechnique = Techniques[TechniqueIndex];

		// Ensure weapon type matches and other conditions are met
		if (SelectedTechnique.bIsUnlocked && TechniqueStruct.TechniquePoints >= SelectedTechnique.PointsRequired)
		{
			// Deduct the required points
			TechniqueStruct.TechniquePoints -= SelectedTechnique.PointsRequired;

			// Play the animation for the technique
			PlayAnimMontage(SelectedTechnique.TechniqueAnimation);

			// Log success
			UE_LOG(LogTemp, Log, TEXT("Technique %s used, %d points deducted."), *SelectedTechnique.TechniqueName, SelectedTechnique.PointsRequired);

			// Only reset gauge if we still have technique points
			if (TechniqueStruct.TechniquePoints < TechniqueStruct.MaxTechniquePoints)
			{
				TechniqueStruct.CurrentGauge = 0.0f; // Reset gauge only if technique points are not maxed out
			}
			else
			{
				TechniqueStruct.CurrentGauge = TechniqueStruct.MaxGauge; // Keep the gauge visually full
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Technique is locked or insufficient technique points!"));
		}
	}
}







void ARen_Low_Poly_Character::IncreaseTechniquePoints(int IncreaseAmount)
{

		TechniqueStruct.TechniquePoints += IncreaseAmount;

}




void ARen_Low_Poly_Character::CheckTechniquePoints()
{
	// Resize the TechniqueAvailability array to match the number of techniques
	TechniqueAvailability.SetNum(Techniques.Num());

	// Loop through each technique to check if it’s available
	for (int32 i = 0; i < Techniques.Num(); i++)
	{
		const FTechnique_Struct& Technique = Techniques[i];

		// Set availability based on points and unlock status
		TechniqueAvailability[i] = (Technique.TechniquePoints >= Technique.PointsRequired && Technique.bIsUnlocked);
	}
}




void ARen_Low_Poly_Character::InitializeWeaponTechniques()
{

	// Clear existing techniques first
	Techniques.Empty();

	if (WeaponType == EWeaponType::Sword)
	{
		// Initialize Sword techniques
		Techniques.Add(FTechnique_Struct{
			TEXT("Stormstrike Flurry"),
			TEXT("Furious multi-strike sword combo."),
			false,                   // Start locked
			StormStrikeFlurryAnimMontage,
			1.6f,                    // Damage bonus
			1,                       // Level required
			1                        // Points required
			});

		Techniques.Add(FTechnique_Struct{
			TEXT("Voltage Breaker"),
			TEXT("Electrifying ground-slam force field."),
			false,
			VoltageBreakerAnimMontage,
			1.3f,
			6,                       // Level required
			2                        // Points required
			});

		Techniques.Add(FTechnique_Struct{
			TEXT("Tempest Barrage"),
			TEXT("Rapid flurry of strikes."),
			false,
			TempestBarrageAnimMontage,
			1.7f,
			10,                      // Level required
			2                        // Points required
			});

		Techniques.Add(FTechnique_Struct{
			TEXT("Static Rush"),
			TEXT("Lightning-infused sword combo."),
			false,
			StaticRushAnimMontage,
			2.4f,
			19,                      // Level required
			3                        // Points required
			});
	}
	else if (WeaponType == EWeaponType::Staff)
	{
		// Initialize Staff techniques
		// Add your staff techniques here similarly
	}


}





void ARen_Low_Poly_Character::UnlockWeaponTechnique(EWeaponType TheWeaponType, int32 CurrentLevel)
{

	// Loop through all techniques
	for (FTechnique_Struct& Technique : Techniques)
	{
		// Check if this technique should be unlocked at current level
		if (CurrentLevel >= Technique.RequiredLevel && !Technique.bIsUnlocked)
		{
			// Unlock the technique
			Technique.bIsUnlocked = true;

			// Log the unlock
			UE_LOG(LogTemp, Warning, TEXT("=== NEW TECHNIQUE UNLOCKED ==="));
			UE_LOG(LogTemp, Warning, TEXT("Weapon Type: %s"), *UEnum::GetValueAsString(TheWeaponType));
			UE_LOG(LogTemp, Warning, TEXT("Technique: %s"), *Technique.TechniqueName);
			UE_LOG(LogTemp, Warning, TEXT("Description: %s"), *Technique.Description);
			UE_LOG(LogTemp, Warning, TEXT("Damage Bonus: %.2f"), Technique.DamageBonus);
			UE_LOG(LogTemp, Warning, TEXT("Points Required: %d"), Technique.PointsRequired);
		}
	}


}








void ARen_Low_Poly_Character::ToggleSoftLock()
{


	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	FVector CharacterForward = GetActorForwardVector();
	FVector CharacterLocation = GetActorLocation();

	AActor* NearestEnemy = nullptr;
	float NearestTargetDistance = SoftLockRange * SoftLockRange;

	for (AActor* Actor : OverlappingActors)
	{
		FVector TargetDirection = Actor->GetActorLocation() - CharacterLocation;
		float TargetDistance = TargetDirection.SizeSquared();
		float AngleToEnemy = FMath::Acos(FVector::DotProduct(TargetDirection.GetSafeNormal(), CharacterForward)) * (180.0f / PI);

		if (TargetDistance <= NearestTargetDistance && AngleToEnemy <= SoftLockAngle)
		{

			bIsSoftLockEnabled = true;
			NearestEnemy = Actor;
			NearestTargetDistance = TargetDistance;
			GEngine->AddOnScreenDebugMessage(1, 1.3f, FColor::Green, TEXT("Soft Lock!"));

		}
	}

	SoftLockedEnemy = NearestEnemy;
	bIsSoftLockEnabled = SoftLockedEnemy != nullptr;


}







void ARen_Low_Poly_Character::TakeDamage(float DamageAmount)
{

	// Apply damage to health
	HealthStruct.TakeDamage(DamageAmount);

	// If the health is <= 0, call the Death function
	if (HealthStruct.CurrentHealth <= 0.0f && !bIsDead)
	{
		Death();  // Call Death only once
	}

	// If damage amount is greater than zero, apply calculated damage
	if (DamageAmount > 0)
	{
		CalculatedDamage = DamageAmount / (1 + TotalDefence);
		HealthStruct.CurrentHealth = FMath::Clamp(HealthStruct.CurrentHealth - CalculatedDamage, 0.0f, HealthStruct.MaxHealth);
		bIsHit = true;

	}

	

}



void ARen_Low_Poly_Character::IncreaseHealth(float HealAmount)
{

	HealthStruct.IncreaseHealth(HealAmount);

}




void ARen_Low_Poly_Character::CalculateTotalAttack()
{

	TotalAttack = BaseAttack * AttackMultiplier;

}



void ARen_Low_Poly_Character::IncreaseAttack(float IncreaseAmount)
{

	BaseAttack *= IncreaseAmount;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Increase Attack"));


}






void ARen_Low_Poly_Character::CalculateTotalDefence()
{

	TotalDefence = BaseDefence * DefenceMultiplier;

}




void ARen_Low_Poly_Character::IncreaseDefence(float IncreaseAmount)
{

	BaseDefence *= IncreaseAmount;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Increase Defence"));

}





void ARen_Low_Poly_Character::CalculateElementalAttack()
{

	TotalElementalAttack = BaseElementalAttack * ElementalMultiplier;

}







void ARen_Low_Poly_Character::SpawnElementalProjectile()
{

	TSubclassOf<AActor> ProjectileClass;


	switch (CurrentElementalAttackType)
	{
	case EElementalAttackType::Fire:
		ProjectileClass = FireProjectileClass;
		break;

	case EElementalAttackType::Ice:
		ProjectileClass = IceProjectileClass;
		break;

	case EElementalAttackType::Thunder:
		ProjectileClass = ThunderProjectileClass;
		break;

		// Add more cases for additional elements here

	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown Elemental Type!"));
		return;
	}


	// Ensure we have a valid projectile class
	if (ProjectileClass)
	{
		// Get the position and rotation from the "projectile point" socket
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp)
		{
			FVector SpawnLocation = MeshComp->GetSocketLocation(TEXT("Switch_Weapon"));

			// Spawn the projectile actor
			GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, GetActorRotation());
		}
	}

	
}



void ARen_Low_Poly_Character::SpawnElementalAOE(FVector SpawnLocation, FRotator SpawnRotation)
{


	TSubclassOf<AActor> AOEClass;


	switch (CurrentElementalAttackType)
	{
	case EElementalAttackType::Fire:
		AOEClass = FireAOEClass;
		break;

	case EElementalAttackType::Ice:
		AOEClass = IceAOEClass;
		break;

	case EElementalAttackType::Thunder:
		AOEClass = ThunderAOEClass;
		break;

		// Add more cases for additional elements here

	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown Elemental Type!"));
		return;
	}


	GetWorld()->SpawnActor<AActor>(AOEClass, SpawnLocation, SpawnRotation);

}




void ARen_Low_Poly_Character::SpawnElementalGround(FVector SpawnLocation, FRotator SpawnRotation)
{


	TSubclassOf<AActor> GroundClass;


	switch (CurrentElementalAttackType)
	{
	case EElementalAttackType::Fire:
		GroundClass = FireGroundClass;
		break;

	case EElementalAttackType::Ice:
		GroundClass = IceGroundClass;
		break;

	case EElementalAttackType::Thunder:
		GroundClass = ThunderGroundClass;
		break;

		// Add more cases for additional elements here

	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown Elemental Type!"));
		return;
	}

	GetWorld()->SpawnActor<AActor>(GroundClass, SpawnLocation, SpawnRotation);


}


void ARen_Low_Poly_Character::UseElementalAttack(const FElemental_Struct& Attack)
{
	FWeaponElementalAttacks* WeaponAttacks = WeaponElementalAttacks.Find(WeaponType);
	if (!WeaponAttacks)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon attacks found for current weapon type"));
		return;
	}

	// Find the attack in the array
	for (FElemental_Struct& SelectedElementalAttack : WeaponAttacks->ElementalAttacks)
	{
		if (SelectedElementalAttack.ElementalAttackName == Attack.ElementalAttackName &&
			SelectedElementalAttack.ElementalType == Attack.ElementalType &&
			SelectedElementalAttack.ElementalLevel == Attack.ElementalLevel)
		{
			// Add debug logging
			UE_LOG(LogTemp, Warning, TEXT("Attempting to use attack: %s"), *SelectedElementalAttack.ElementalAttackName);
			UE_LOG(LogTemp, Warning, TEXT("Attack status - Unlocked: %s, Current Mana: %.1f, Required Mana: %.1f"),
				SelectedElementalAttack.bIsUnlocked ? TEXT("True") : TEXT("False"),
				ManaStruct.CurrentMana,
				SelectedElementalAttack.ManaCost);

			// Check proficiency level requirement
			bool bHasRequiredLevel = false;
			const FElemental_Proficiency_Struct& ProficiencyStruct =
				WeaponElementalProficiency.ElementalWeaponProficiencyMap[WeaponType];

			switch (SelectedElementalAttack.ElementalType)
			{
			case EElementalAttackType::Fire:
				bHasRequiredLevel = SelectedElementalAttack.ElementalLevel <= ProficiencyStruct.FireLevel;
				break;
			case EElementalAttackType::Ice:
				bHasRequiredLevel = SelectedElementalAttack.ElementalLevel <= ProficiencyStruct.IceLevel;
				break;
			case EElementalAttackType::Thunder:
				bHasRequiredLevel = SelectedElementalAttack.ElementalLevel <= ProficiencyStruct.ThunderLevel;
				break;
			}

			if (bHasRequiredLevel && ManaStruct.CurrentMana >= SelectedElementalAttack.ManaCost)
			{
				// Deduct mana
				ManaStruct.CurrentMana -= SelectedElementalAttack.ManaCost;

				// Play the animation for the attack
				PlayAnimMontage(SelectedElementalAttack.Elemental_Attack_Animation);

				// Set the current elemental attack type
				CurrentElementalAttackType = SelectedElementalAttack.ElementalType;

				// Add experience
				AddExperienceToElementalProfiency(WeaponType, SelectedElementalAttack.ElementalType, 90.0f);

				UE_LOG(LogTemp, Warning, TEXT("Successfully used attack: %s"), *SelectedElementalAttack.ElementalAttackName);
				UE_LOG(LogTemp, Warning, TEXT("Mana Cost: %.1f, Current Mana: %.1f"),
					SelectedElementalAttack.ManaCost, ManaStruct.CurrentMana);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cannot use %s - Has Required Level: %s, Has Enough Mana: %s"),
					*SelectedElementalAttack.ElementalAttackName,
					bHasRequiredLevel ? TEXT("True") : TEXT("False"),
					(ManaStruct.CurrentMana >= SelectedElementalAttack.ManaCost) ? TEXT("True") : TEXT("False"));
			}
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Attack not found in weapon attacks array"));
}



void ARen_Low_Poly_Character::AddExperienceToElementalProfiency(EWeaponType TheWeaponType, EElementalAttackType ElementType, float EXPAmount)
{
	FElemental_Proficiency_Struct* ProficiencyStruct = nullptr;

	// Check if the weapon type exists in the map first
	if (WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(TheWeaponType))
	{
		ProficiencyStruct = &WeaponElementalProficiency.ElementalWeaponProficiencyMap[TheWeaponType];
		UE_LOG(LogTemp, Warning, TEXT("Proficiency Struct: %s"), *UEnum::GetValueAsString(TheWeaponType));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon type %s not found in proficiency map!"), *UEnum::GetValueAsString(TheWeaponType));
		return;  // Exit the function early if the weapon type doesn't exist
	}

	if (ProficiencyStruct)
	{
		// Logic for Fire Proficiency
		if (ElementType == EElementalAttackType::Fire)
		{
			ProficiencyStruct->FireProficiency += EXPAmount;
			UE_LOG(LogTemp, Warning, TEXT("Added %.2f EXP to Fire proficiency. New Proficiency: %.2f"), EXPAmount, ProficiencyStruct->FireProficiency);

			// Check if Fire proficiency has leveled up
			int32 CurrentLevel = ProficiencyStruct->FireLevel;
			UE_LOG(LogTemp, Warning, TEXT("Current Fire Level: %d"), CurrentLevel);

			if (ProficiencyStruct->FireProficiencyThresholds.Contains(CurrentLevel))
			{
				float ThresholdValue = ProficiencyStruct->FireProficiencyThresholds[CurrentLevel];
				UE_LOG(LogTemp, Warning, TEXT("Fire Level: %d, Threshold Value: %.2f"), CurrentLevel, ThresholdValue);

				if (ProficiencyStruct->FireProficiency >= ThresholdValue)
				{
					ProficiencyStruct->FireLevel++;
					ProficiencyStruct->FireProficiency -= ThresholdValue;


					// Set a timer to call UnlockElementalAbility after 1 second
					GetWorld()->GetTimerManager().SetTimer(TimerHandle_UnlockElementalAbility, FTimerDelegate::CreateUObject(this, &ARen_Low_Poly_Character::UnlockElementalAbility, TheWeaponType, EElementalAttackType::Fire), 1.0f, false);

					UE_LOG(LogTemp, Warning, TEXT("Fire proficiency leveled up! New Level: %d"), ProficiencyStruct->FireLevel);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Threshold for Fire Level %d not found!"), CurrentLevel);
			}
		}
		// Logic for Ice Proficiency
		else if (ElementType == EElementalAttackType::Ice)
		{
			ProficiencyStruct->IceProficiency += EXPAmount;
			UE_LOG(LogTemp, Warning, TEXT("Added %.2f EXP to Ice proficiency. New Proficiency: %.2f"), EXPAmount, ProficiencyStruct->IceProficiency);

			// Check if Ice proficiency has leveled up
			int32 CurrentLevel = ProficiencyStruct->IceLevel;
			UE_LOG(LogTemp, Warning, TEXT("Current Ice Level: %d"), CurrentLevel);

			if (ProficiencyStruct->IceProficiencyThresholds.Contains(CurrentLevel))
			{
				float ThresholdValue = ProficiencyStruct->IceProficiencyThresholds[CurrentLevel];
				UE_LOG(LogTemp, Warning, TEXT("Ice Level: %d, Threshold Value: %.2f"), CurrentLevel, ThresholdValue);

				if (ProficiencyStruct->IceProficiency >= ThresholdValue)
				{
					ProficiencyStruct->IceLevel++;
					ProficiencyStruct->IceProficiency -= ThresholdValue;

					// Set a timer to call UnlockElementalAbility after 1 second
					GetWorld()->GetTimerManager().SetTimer(TimerHandle_UnlockElementalAbility, FTimerDelegate::CreateUObject(this, &ARen_Low_Poly_Character::UnlockElementalAbility, TheWeaponType, EElementalAttackType::Ice), 1.0f, false);

					UE_LOG(LogTemp, Warning, TEXT("Ice proficiency leveled up! New Level: %d"), ProficiencyStruct->IceLevel);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Threshold for Ice Level %d not found!"), CurrentLevel);
			}
		}
		// Logic for Thunder Proficiency
		else if (ElementType == EElementalAttackType::Thunder)
		{
			ProficiencyStruct->ThunderProficiency += EXPAmount;
			UE_LOG(LogTemp, Warning, TEXT("Added %.2f EXP to Thunder proficiency. New Proficiency: %.2f"), EXPAmount, ProficiencyStruct->ThunderProficiency);

			// Check if Thunder proficiency has leveled up
			int32 CurrentLevel = ProficiencyStruct->ThunderLevel;
			UE_LOG(LogTemp, Warning, TEXT("Current Thunder Level: %d"), CurrentLevel);

			if (ProficiencyStruct->ThunderProficiencyThresholds.Contains(CurrentLevel))
			{
				float ThresholdValue = ProficiencyStruct->ThunderProficiencyThresholds[CurrentLevel];
				UE_LOG(LogTemp, Warning, TEXT("Thunder Level: %d, Threshold Value: %.2f"), CurrentLevel, ThresholdValue);

				if (ProficiencyStruct->ThunderProficiency >= ThresholdValue)
				{
					ProficiencyStruct->ThunderLevel++;
					ProficiencyStruct->ThunderProficiency -= ThresholdValue;

					// Set a timer to call UnlockElementalAbility after 1 second
					GetWorld()->GetTimerManager().SetTimer(TimerHandle_UnlockElementalAbility, FTimerDelegate::CreateUObject(this, &ARen_Low_Poly_Character::UnlockElementalAbility, TheWeaponType, EElementalAttackType::Thunder), 1.0f, false);

					UE_LOG(LogTemp, Warning, TEXT("Thunder proficiency leveled up! New Level: %d"), ProficiencyStruct->ThunderLevel);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Threshold for Thunder Level %d not found!"), CurrentLevel);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Proficiency struct is null for WeaponType=%d"), static_cast<int32>(TheWeaponType));
	}
}




void ARen_Low_Poly_Character::UnlockElementalAbility(EWeaponType TheWeaponType, EElementalAttackType ElementType)
{
	// Find the appropriate attacks array for the weapon type
	if (!WeaponElementalAttacks.Contains(TheWeaponType))
	{
		WeaponElementalAttacks.Add(TheWeaponType, FWeaponElementalAttacks());
	}

	FWeaponElementalAttacks& WeaponAttacksRef = WeaponElementalAttacks[TheWeaponType];
	int32 CurrentLevel = 0;
	switch (ElementType)
	{
	case EElementalAttackType::Fire:
		CurrentLevel = WeaponElementalProficiency.ElementalWeaponProficiencyMap[TheWeaponType].FireLevel;
		break;
	case EElementalAttackType::Ice:
		CurrentLevel = WeaponElementalProficiency.ElementalWeaponProficiencyMap[TheWeaponType].IceLevel;
		break;
	case EElementalAttackType::Thunder:
		CurrentLevel = WeaponElementalProficiency.ElementalWeaponProficiencyMap[TheWeaponType].ThunderLevel;
		break;
	}

	// Create and explicitly set unlock status for the new ability
	FElemental_Struct NewAbility;
	NewAbility.bIsUnlocked = true;  // Explicitly set to true
	NewAbility.ManaCost = 20.0f;    // Set a default mana cost

	if (TheWeaponType == EWeaponType::Sword)
	{
		switch (ElementType)
		{
		case EElementalAttackType::Fire:
			if (CurrentLevel == 2)
			{
				NewAbility = FElemental_Struct(TEXT("Fire Lv.2"), ElementType, 2.0f, 20.0f, 2, true, FireAOEAnimation,
					TEXT("Creates an explosion, burns enemies for longer."));
				NewAbility.ManaCost = 30.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Fire Lv.2 Ability for Sword!"));
			}
			else if (CurrentLevel == 3)
			{
				NewAbility = FElemental_Struct(TEXT("Fire Lv.3"), ElementType, 2.5f, 30.0f, 3, true, FireGroundAnimation,
					TEXT("Summons molten spikes, burns enemies for an extended time."));
				NewAbility.ManaCost = 45.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Fire Lv.3 Ability for Sword!"));
			}
			break;

		case EElementalAttackType::Ice:
			if (CurrentLevel == 2)
			{
				NewAbility = FElemental_Struct(TEXT("Ice Lv.2"), ElementType, 2.0f, 20.0f, 2, true, IceAOEAnimation,
					TEXT("Summons ice shards, freezing enemies for longer."));
				NewAbility.ManaCost = 30.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Ice Lv.2 Ability for Sword!"));
			}
			else if (CurrentLevel == 3)
			{
				NewAbility = FElemental_Struct(TEXT("Ice Lv.3"), ElementType, 2.5f, 30.0f, 3, true, IceGroundAnimation,
					TEXT("Summons ice spiral, freezing enemies for an extended time."));
				NewAbility.ManaCost = 45.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Ice Lv.3 Ability for Sword!"));
			}
			break;

		case EElementalAttackType::Thunder:
			if (CurrentLevel == 2)
			{
				NewAbility = FElemental_Struct(TEXT("Thunder Lv.2"), ElementType, 2.0f, 20.0f, 2, true, ThunderAOEAnimation,
					TEXT("Summons lightning, stunning enemies for longer."));
				NewAbility.ManaCost = 30.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Thunder Lv.2 Ability for Sword!"));
			}
			else if (CurrentLevel == 3)
			{
				NewAbility = FElemental_Struct(TEXT("Thunder Lv.3"), ElementType, 2.5f, 30.0f, 3, true, ThunderGroundAnimation,
					TEXT("Summons lightning hoop, stunning enemies for an extended time."));
				NewAbility.ManaCost = 45.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Thunder Lv.3 Ability for Sword!"));
			}
			break;
		}
	}
	else if (TheWeaponType == EWeaponType::Staff)
	{
		switch (ElementType)
		{
		case EElementalAttackType::Fire:
			if (CurrentLevel == 2)
			{
				NewAbility = FElemental_Struct(TEXT("Fire Lv.2"), ElementType, 2.2f, 20.0f, 2, true, FireAOEAnimation,
					TEXT("Creates a larger explosion, burns enemies for longer."));
				NewAbility.ManaCost = 25.0f;  // Staff uses less mana
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Fire Lv.2 Ability for Staff!"));
			}
			else if (CurrentLevel == 3)
			{
				NewAbility = FElemental_Struct(TEXT("Fire Lv.3"), ElementType, 2.8f, 30.5f, 3, true, FireGroundAnimation,
					TEXT("Summons multiple molten spikes, burns enemies for an extended time."));
				NewAbility.ManaCost = 40.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Fire Lv.3 Ability for Staff!"));
			}
			break;

		case EElementalAttackType::Ice:
			if (CurrentLevel == 2)
			{
				NewAbility = FElemental_Struct(TEXT("Ice Lv.2"), ElementType, 2.0f, 20.0f, 2, true, IceAOEAnimation,
					TEXT("Creates a frost nova, freezing multiple enemies."));
				NewAbility.ManaCost = 25.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Ice Lv.2 Ability for Staff!"));
			}
			else if (CurrentLevel == 3)
			{
				NewAbility = FElemental_Struct(TEXT("Ice Lv.3"), ElementType, 2.5f, 30.0f, 3, true, IceGroundAnimation,
					TEXT("Summons an ice storm, freezing all nearby enemies."));
				NewAbility.ManaCost = 40.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Ice Lv.3 Ability for Staff!"));
			}
			break;

		case EElementalAttackType::Thunder:
			if (CurrentLevel == 2)
			{
				NewAbility = FElemental_Struct(TEXT("Thunder Lv.2"), ElementType, 2.0f, 20.0f, 2, true, ThunderAOEAnimation,
					TEXT("Calls down chain lightning, stunning multiple enemies."));
				NewAbility.ManaCost = 25.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Thunder Lv.2 Ability for Staff!"));
			}
			else if (CurrentLevel == 3)
			{
				NewAbility = FElemental_Struct(TEXT("Thunder Lv.3"), ElementType, 2.5f, 30.0f, 3, true, ThunderGroundAnimation,
					TEXT("Creates a thunder storm, stunning all nearby enemies."));
				NewAbility.ManaCost = 40.0f;
				UE_LOG(LogTemp, Warning, TEXT("Unlocked Thunder Lv.3 Ability for Staff!"));
			}
			break;
		}
	}

	// Only add if we created a valid ability
	if (NewAbility.ElementalAttackName != TEXT(""))
	{
		// Double check that the ability isn't already in the list
		bool bExists = false;
		for (const FElemental_Struct& Attack : WeaponAttacksRef.ElementalAttacks)
		{
			if (Attack.ElementalAttackName == NewAbility.ElementalAttackName)
			{
				bExists = true;
				break;
			}
		}

		if (!bExists)
		{
			NewAbility.bIsUnlocked = true;  // Double-check it's set
			WeaponAttacksRef.ElementalAttacks.Add(NewAbility);
			UE_LOG(LogTemp, Warning, TEXT("Added new ability: %s (Unlocked: %s, Mana Cost: %.1f)"),
				*NewAbility.ElementalAttackName,
				NewAbility.bIsUnlocked ? TEXT("True") : TEXT("False"),
				NewAbility.ManaCost);
		}
	}
}



void ARen_Low_Poly_Character::AddElementalAttackDelayed(const FElemental_Struct& ElementalAttack, EWeaponType TheWeaponType)
{

	//ElementalAttacks.Add(ElementalAttack);

 // Check if the WeaponType exists in the map, if not, initialize it
	if (!WeaponElementalAttacks.Contains(TheWeaponType))
	{
		WeaponElementalAttacks.Emplace(TheWeaponType, FWeaponElementalAttacks()); // Use Emplace for more efficient initialization
	}

	FElemental_Struct UnlockedAttack = ElementalAttack;
	UnlockedAttack.bIsUnlocked = true;

	// Add the new Elemental Attack to the corresponding weapon's array
	WeaponElementalAttacks[TheWeaponType].ElementalAttacks.Add(UnlockedAttack);



	// Extensive logging
	UE_LOG(LogTemp, Warning, TEXT("Adding New Elemental Attack:"));
	UE_LOG(LogTemp, Warning, TEXT("  Weapon Type: %s"), *UEnum::GetValueAsString(TheWeaponType));
	UE_LOG(LogTemp, Warning, TEXT("  Attack Name: %s"), *ElementalAttack.ElementalAttackName);
	UE_LOG(LogTemp, Warning, TEXT("  Elemental Type: %s"), *UEnum::GetValueAsString(ElementalAttack.ElementalType));
	UE_LOG(LogTemp, Warning, TEXT("  Attack Level: %d"), ElementalAttack.ElementalLevel);
	UE_LOG(LogTemp, Warning, TEXT("  Is Unlocked: %s"), ElementalAttack.bIsUnlocked ? TEXT("TRUE") : TEXT("FALSE"));


	// Log the number of attacks for that weapon type
	UE_LOG(LogTemp, Warning, TEXT("Added new elemental attack to weapon type %d. Total attacks: %d"),
		(int32)TheWeaponType, WeaponElementalAttacks[TheWeaponType].ElementalAttacks.Num());

}



void ARen_Low_Poly_Character::ApplyTheBurnEffect(AEnemy_Poly* Enemy, float Duration, float DamagePerSecondss)
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Burn effect failed: Enemy is null"));
		return;
	}

	if (Enemy->bIsBurning)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is already burning"), *Enemy->GetName());
		return;
	}

	// Set the burn effect state
	Enemy->bIsBurning = true;
	Enemy->BurnDurationRemaining = Duration;

	// Set the overlay material to visually indicate burn
	if (BurnOverlayMaterial)
	{
		Enemy->GetMesh()->SetOverlayMaterial(BurnOverlayMaterial);
	}

	// Start a repeating timer specific to this enemy
	GetWorld()->GetTimerManager().SetTimer(
		Enemy->BurnTimerHandle,  // Use the enemy's unique timer handle
		[Enemy, DamagePerSecondss, this]()
		{
			if (!Enemy || !Enemy->IsValidLowLevel())
			{
				// Stop the timer if the enemy is no longer valid
				UE_LOG(LogTemp, Warning, TEXT("Burn effect ended: Enemy is no longer valid."));
				GetWorld()->GetTimerManager().ClearTimer(Enemy->BurnTimerHandle);
				return;
			}

			// Apply damage
			Enemy->ApplyDamage(DamagePerSecondss, FHitResult(), GetController(), this);
			UE_LOG(LogTemp, Log, TEXT("Applying %f burn damage to %s"), DamagePerSecondss, *Enemy->GetName());

			// Reduce remaining duration correctly
			Enemy->BurnDurationRemaining -= 2.0f;

			if (Enemy->BurnDurationRemaining <= 0.0f)
			{
				// Stop the timer when the effect ends
				GetWorld()->GetTimerManager().ClearTimer(Enemy->BurnTimerHandle);
				Enemy->bIsBurning = false;

				// Reset the overlay material
				Enemy->GetMesh()->SetOverlayMaterial(nullptr);

				UE_LOG(LogTemp, Log, TEXT("Burn effect ended for %s"), *Enemy->GetName());
			}
		},
		2.0f,  // Interval: Apply damage every 2 seconds
			true   // Loop the timer
			);

	// Debug log for confirmation
	UE_LOG(LogTemp, Log, TEXT("Burn effect applied to %s for %f seconds at %f DPS"), *Enemy->GetName(), Duration, DamagePerSecondss);
}











void ARen_Low_Poly_Character::ApplyFreezeEffect(AEnemy_Poly* Enemy, float Duration)
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Freeze effect failed: Enemy is null"));
		return;
	}

	// Check if the enemy is already frozen
	if (Enemy->bIsFrozen)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is already frozen"), *Enemy->GetName());
		return;
	}

	// Mark the enemy as frozen
	Enemy->bIsFrozen = true;

	// Set the freeze overlay material for visual feedback
	if (FreezeOverlayMaterial)
	{
		Enemy->GetMesh()->SetOverlayMaterial(FreezeOverlayMaterial);
	}

	// Get the AI controller of the enemy
	AEnemy_AIController* EnemyAIController = Cast<AEnemy_AIController>(Enemy->GetController());
	if (EnemyAIController)
	{
		// Disable AI logic
		EnemyAIController->DisableAI();

		// Stop the enemy from facing the player or performing actions
		Enemy->bShouldFacePlayer = false;
		EnemyAIController->SetFrozenState(true);

		// Pause animations for a frozen effect
		Enemy->GetMesh()->bPauseAnims = true;

		// Set a timer to re-enable AI and clean up after the freeze duration
		GetWorld()->GetTimerManager().SetTimer(
			Enemy->FreezeTimerHandle,  // Use the enemy's freeze timer handle
			[Enemy, EnemyAIController]()
			{
				if (Enemy && EnemyAIController)
				{
					// Re-enable AI and reset state
					EnemyAIController->RestartAI();
					EnemyAIController->SetFrozenState(false);

					// Allow the enemy to face the player again
					Enemy->bShouldFacePlayer = true;

					// Resume animations
					Enemy->GetMesh()->bPauseAnims = false;

					// Reset the overlay material to its original state
					Enemy->GetMesh()->SetOverlayMaterial(nullptr);

					// Clear frozen state
					Enemy->bIsFrozen = false;

					UE_LOG(LogTemp, Log, TEXT("Freeze effect ended for %s"), *Enemy->GetName());
				}
			},
			Duration,  // Freeze duration
				false  // Do not loop
				);

		// Debug log
		UE_LOG(LogTemp, Log, TEXT("Freeze effect applied to %s for %f seconds"), *Enemy->GetName(), Duration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get AI Controller for %s"), *Enemy->GetName());
	}
}





void ARen_Low_Poly_Character::ApplyStunEffect(AEnemy_Poly* Enemy, float Duration)
{

	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stun effect failed: Enemy is null"));
		return;
	}

	// Check if the enemy is already stunned
	if (Enemy->bIsStunned)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is already stunned"), *Enemy->GetName());
		return;
	}

	// Mark enemy as stunned
	Enemy->bIsStunned = true;

	// Set the overlay material to visually indicate stun
	if (StunOverlayMaterial)
	{
		Enemy->GetMesh()->SetOverlayMaterial(StunOverlayMaterial);
	}

	// Get the AI controller of the enemy
	AEnemy_AIController* EnemyAIController = Cast<AEnemy_AIController>(Enemy->GetController());
	if (EnemyAIController)
	{
		// Disable the AI
		EnemyAIController->DisableAI();
		EnemyAIController->bIsStunned = true;

		// Set a timer to re-enable AI and clean up after the stun duration
		GetWorld()->GetTimerManager().SetTimer(
			Enemy->StunTimerHandle,  // Use the enemy's timer handle
			[Enemy, EnemyAIController]()
			{
				if (Enemy && EnemyAIController)
				{
					// Re-enable AI
					EnemyAIController->RestartAI();

					// Reset overlay material
					Enemy->GetMesh()->SetOverlayMaterial(nullptr);

					// Clear stun state
					Enemy->bIsStunned = false;
					EnemyAIController->bIsStunned = false;


					UE_LOG(LogTemp, Log, TEXT("Stun effect ended for %s"), *Enemy->GetName());
				}
			},
			Duration,  // Stun duration
				false  // Do not loop
				);

		// Debug log
		UE_LOG(LogTemp, Log, TEXT("Stun effect applied to %s for %f seconds"), *Enemy->GetName(), Duration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get AI Controller for %s"), *Enemy->GetName());
	}

}




void ARen_Low_Poly_Character::InitialiseElementalAttacks()
{
	// IMPORTANT: Only clear if no attacks are present
	if (WeaponElementalAttacks.Num() == 0)
	{
		WeaponElementalAttacks.Empty();
	}

	UE_LOG(LogTemp, Error, TEXT("=== INITIALIZING ELEMENTAL ATTACKS ==="));

	// Initialize attacks for each weapon type
	for (auto& WeaponPair : WeaponElementalProficiency.ElementalWeaponProficiencyMap)
	{
		EWeaponType CurrentWeaponType = WeaponPair.Key;
		const FElemental_Proficiency_Struct& ProficiencyStruct = WeaponPair.Value;

		UE_LOG(LogTemp, Error, TEXT("Initializing Attacks for Weapon Type: %s"), *UEnum::GetValueAsString(CurrentWeaponType));
		UE_LOG(LogTemp, Error, TEXT("Current Proficiency Levels - Fire: %d, Ice: %d, Thunder: %d"),
			ProficiencyStruct.FireLevel,
			ProficiencyStruct.IceLevel,
			ProficiencyStruct.ThunderLevel);

		// Only add attacks if the weapon type doesn't already have attacks
		if (!WeaponElementalAttacks.Contains(CurrentWeaponType))
		{
			// Create new FWeaponElementalAttacks for this weapon type
			FWeaponElementalAttacks NewWeaponAttacks;
			TSet<FString> AddedAttacks; // Track added attacks to prevent duplicates

			if (CurrentWeaponType == EWeaponType::Sword)
			{
				// Helper function to add attack if not already present
				auto AddUniqueAttack = [&](const FElemental_Struct& Attack) {
					FString AttackKey = FString::Printf(TEXT("%s_Lv%d"),
						*Attack.ElementalAttackName, Attack.ElementalLevel);
					if (!AddedAttacks.Contains(AttackKey))
					{
						NewWeaponAttacks.ElementalAttacks.Add(Attack);
						AddedAttacks.Add(AttackKey);

						// Log each added attack
						UE_LOG(LogTemp, Error, TEXT("Added Attack: %s (Level %d, Type %s)"),
							*Attack.ElementalAttackName,
							Attack.ElementalLevel,
							*UEnum::GetValueAsString(Attack.ElementalType));
					}
				};

				// Add base attacks
				AddUniqueAttack(FElemental_Struct(TEXT("Fire"), EElementalAttackType::Fire, 1.5f, 10.0f, 1, true, FireProjectileAnimation, TEXT("Burn enemies over time")));
				AddUniqueAttack(FElemental_Struct(TEXT("Ice"), EElementalAttackType::Ice, 1.6f, 20.0f, 1, true, IceProjectileAnimation, TEXT("Freeze enemies over time")));
				AddUniqueAttack(FElemental_Struct(TEXT("Thunder"), EElementalAttackType::Thunder, 1.8f, 20.0f, 1, true, ThunderProjectileAnimation, TEXT("Stun enemies over time")));

				// Add unlocked abilities based on levels
				if (ProficiencyStruct.FireLevel >= 2)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Fire Lv.2"), EElementalAttackType::Fire, 2.0f, 20.0f, 2, true, FireAOEAnimation, TEXT("Creates an explosion, burns enemies for longer.")));
				}
				if (ProficiencyStruct.FireLevel >= 3)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Fire Lv.3"), EElementalAttackType::Fire, 2.5f, 30.0f, 3, true, FireGroundAnimation, TEXT("Summons molten spikes, burns enemies for an extended time.")));
				}

				// Add unlocked abilities based on levels
				if (ProficiencyStruct.IceLevel >= 2)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Ice Lv.2"), EElementalAttackType::Ice, 2.0f, 20.0f, 2, true, IceAOEAnimation, TEXT("Summons ice shards, freezing enemies for longer.")));
				}
				if (ProficiencyStruct.IceLevel >= 3)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Ice Lv.3"), EElementalAttackType::Ice, 2.5f, 30.0f, 3, true, IceGroundAnimation, TEXT("Summons ice spiral, freezing enemies for an extended time.")));
				}

				// Add unlocked abilities based on levels
				if (ProficiencyStruct.ThunderLevel >= 2)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Thunder Lv.2"), EElementalAttackType::Thunder, 2.0f, 20.0f, 2, true, ThunderAOEAnimation, TEXT("Summons lightning, stunning enemies for longer.")));
				}
				if (ProficiencyStruct.ThunderLevel >= 3)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Thunder Lv.3"), EElementalAttackType::Thunder, 2.5f, 30.0f, 3, true, ThunderGroundAnimation, TEXT("Summons lightning hoop, stunning enemies for an extended time.")));
				}
			}
			else if (CurrentWeaponType == EWeaponType::Staff)
			{
				// Similar logic for Staff, but with Staff-specific attacks
				auto AddUniqueAttack = [&](const FElemental_Struct& Attack) {
					FString AttackKey = FString::Printf(TEXT("%s_Lv%d"),
						*Attack.ElementalAttackName, Attack.ElementalLevel);
					if (!AddedAttacks.Contains(AttackKey))
					{
						NewWeaponAttacks.ElementalAttacks.Add(Attack);
						AddedAttacks.Add(AttackKey);
					}
				};

				// Add base Staff attacks
				AddUniqueAttack(FElemental_Struct(TEXT("Fire"), EElementalAttackType::Fire, 1.7f, 15.0f, 1, true, FireProjectileAnimation, TEXT("Burns enemies over time.")));
				AddUniqueAttack(FElemental_Struct(TEXT("Ice"), EElementalAttackType::Ice, 1.9f, 15.0f, 1, true, IceProjectileAnimation, TEXT("Freezes enemies over time.")));
				AddUniqueAttack(FElemental_Struct(TEXT("Thunder"), EElementalAttackType::Thunder, 1.5f, 10.0f, 1, true, ThunderProjectileAnimation, TEXT("Stuns enemies over time.")));

				// Add Staff-specific level 2 and 3 attacks based on proficiency
				if (ProficiencyStruct.FireLevel >= 2)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Fire Lv.2"), EElementalAttackType::Fire, 2.2f, 20.0f, 2, true, FireAOEAnimation, TEXT("Creates an explosion, burns enemies for longer.")));
				}
				if (ProficiencyStruct.FireLevel >= 3)
				{
					AddUniqueAttack(FElemental_Struct(TEXT("Fire Lv.3"), EElementalAttackType::Fire, 2.8f, 30.5f, 3, true, FireGroundAnimation, TEXT("Summons molten spikes, burns enemies for an extended time.")));
				}

				// Similar additions for Ice and Thunder attacks for Staff
			}

			// Add the initialized attacks to the map
			WeaponElementalAttacks.Add(CurrentWeaponType, NewWeaponAttacks);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Attacks already exist for %s, skipping initialization"), *UEnum::GetValueAsString(CurrentWeaponType));
		}

		// Log total attacks for this weapon type
		UE_LOG(LogTemp, Error, TEXT("Total Attacks for %s: %d"),
			*UEnum::GetValueAsString(CurrentWeaponType),
			WeaponElementalAttacks[CurrentWeaponType].ElementalAttacks.Num());
	}

	UE_LOG(LogTemp, Error, TEXT("=== FINISHED INITIALIZING ELEMENTAL ATTACKS ==="));

}



void ARen_Low_Poly_Character::InitialiseDefaultElementalProficiencyValues()
{
	// Initialize Sword
	FElemental_Proficiency_Struct SwordProficiency;
	SwordProficiency.FireProficiencyThresholds.Add(1, 100.f);
	SwordProficiency.FireProficiencyThresholds.Add(2, 200.f);
	SwordProficiency.FireProficiencyThresholds.Add(3, 300.f);
	UE_LOG(LogTemp, Warning, TEXT("Sword Fire Thresholds initialized with %d entries"), SwordProficiency.FireProficiencyThresholds.Num());

	SwordProficiency.IceProficiencyThresholds.Add(1, 100.f);
	SwordProficiency.IceProficiencyThresholds.Add(2, 200.f);
	SwordProficiency.IceProficiencyThresholds.Add(3, 300.f);
	UE_LOG(LogTemp, Warning, TEXT("Sword Ice Thresholds initialized with %d entries"), SwordProficiency.IceProficiencyThresholds.Num());

	SwordProficiency.ThunderProficiencyThresholds.Add(1, 100.f);
	SwordProficiency.ThunderProficiencyThresholds.Add(2, 200.f);
	SwordProficiency.ThunderProficiencyThresholds.Add(3, 300.f);
	UE_LOG(LogTemp, Warning, TEXT("Sword Thunder Thresholds initialized with %d entries"), SwordProficiency.ThunderProficiencyThresholds.Num());

	WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Sword, SwordProficiency);
	UE_LOG(LogTemp, Warning, TEXT("Sword proficiency added to map"));

	// Initialize Staff
	FElemental_Proficiency_Struct StaffProficiency;
	StaffProficiency.FireProficiencyThresholds.Add(1, 100.f);
	StaffProficiency.FireProficiencyThresholds.Add(2, 200.f);
	StaffProficiency.FireProficiencyThresholds.Add(3, 300.f);
	UE_LOG(LogTemp, Warning, TEXT("Staff Fire Thresholds initialized with %d entries"), StaffProficiency.FireProficiencyThresholds.Num());

	StaffProficiency.IceProficiencyThresholds.Add(1, 100.f);
	StaffProficiency.IceProficiencyThresholds.Add(2, 200.f);
	StaffProficiency.IceProficiencyThresholds.Add(3, 300.f);
	UE_LOG(LogTemp, Warning, TEXT("Staff Ice Thresholds initialized with %d entries"), StaffProficiency.IceProficiencyThresholds.Num());

	StaffProficiency.ThunderProficiencyThresholds.Add(1, 100.f);
	StaffProficiency.ThunderProficiencyThresholds.Add(2, 200.f);
	StaffProficiency.ThunderProficiencyThresholds.Add(3, 300.f);
	UE_LOG(LogTemp, Warning, TEXT("Staff Thunder Thresholds initialized with %d entries"), StaffProficiency.ThunderProficiencyThresholds.Num());

	WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Staff, StaffProficiency);
	UE_LOG(LogTemp, Warning, TEXT("Staff proficiency added to map"));

	UE_LOG(LogTemp, Warning, TEXT("Final map contains %d entries"), WeaponElementalProficiency.ElementalWeaponProficiencyMap.Num());
}



void ARen_Low_Poly_Character::InitialiseElementalProficiencies()
{
	// Ensure Sword thresholds are initialized only if missing
	if (!WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Sword))
	{
		WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Sword, FElemental_Proficiency_Struct());
	}

	FElemental_Proficiency_Struct& SwordProficiency = WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Sword];

	if (SwordProficiency.FireProficiencyThresholds.Num() == 0)
	{
		SwordProficiency.FireProficiencyThresholds.Add(1, 100.f);
		SwordProficiency.FireProficiencyThresholds.Add(2, 200.f);
		SwordProficiency.FireProficiencyThresholds.Add(3, 300.f);
	}
	if (SwordProficiency.IceProficiencyThresholds.Num() == 0)
	{
		SwordProficiency.IceProficiencyThresholds.Add(1, 100.f);
		SwordProficiency.IceProficiencyThresholds.Add(2, 200.f);
		SwordProficiency.IceProficiencyThresholds.Add(3, 300.f);
	}
	if (SwordProficiency.ThunderProficiencyThresholds.Num() == 0)
	{
		SwordProficiency.ThunderProficiencyThresholds.Add(1, 100.f);
		SwordProficiency.ThunderProficiencyThresholds.Add(2, 200.f);
		SwordProficiency.ThunderProficiencyThresholds.Add(3, 300.f);
	}

	// Ensure Staff thresholds are initialized only if missing
	if (!WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Staff))
	{
		WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Staff, FElemental_Proficiency_Struct());
	}

	FElemental_Proficiency_Struct& StaffProficiency = WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Staff];

	if (StaffProficiency.FireProficiencyThresholds.Num() == 0)
	{
		StaffProficiency.FireProficiencyThresholds.Add(1, 100.f);
		StaffProficiency.FireProficiencyThresholds.Add(2, 200.f);
		StaffProficiency.FireProficiencyThresholds.Add(3, 300.f);
	}
	if (StaffProficiency.IceProficiencyThresholds.Num() == 0)
	{
		StaffProficiency.IceProficiencyThresholds.Add(1, 100.f);
		StaffProficiency.IceProficiencyThresholds.Add(2, 200.f);
		StaffProficiency.IceProficiencyThresholds.Add(3, 300.f);
	}
	if (StaffProficiency.ThunderProficiencyThresholds.Num() == 0)
	{
		StaffProficiency.ThunderProficiencyThresholds.Add(1, 100.f);
		StaffProficiency.ThunderProficiencyThresholds.Add(2, 200.f);
		StaffProficiency.ThunderProficiencyThresholds.Add(3, 300.f);
	}
}





void ARen_Low_Poly_Character::EnsureAllInitialisation()
{

	if (WeaponElementalProficiency.ElementalWeaponProficiencyMap.Num() == 0)
	{
		InitialiseElementalProficiencies();
	}

	InitialiseElementalAttacks();

	
	VerifyInitialisation();
	VerifyElementalAttacks(); // Add this line
	VerifyInitialisation();
}




void ARen_Low_Poly_Character::VerifyInitialisation()
{


	bool bHasSword = WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Sword);
	bool bHasStaff = WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(EWeaponType::Staff);

	UE_LOG(LogTemp, Log, TEXT("Verification - Sword: %s, Staff: %s"),
		bHasSword ? TEXT("Present") : TEXT("Missing"),
		bHasStaff ? TEXT("Present") : TEXT("Missing"));

	if (!bHasSword || !bHasStaff)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing weapon types after initialization. Reinitializing..."));
		InitialiseElementalProficiencies();
	}


}



void ARen_Low_Poly_Character::VerifyElementalAttacks()
{

	for (const auto& WeaponPair : WeaponElementalProficiency.ElementalWeaponProficiencyMap)
	{
		EWeaponType CurrentWeaponType = WeaponPair.Key;
		const FElemental_Proficiency_Struct& ProficiencyStruct = WeaponPair.Value;

		// Check if this weapon type has its attacks initialized
		if (!WeaponElementalAttacks.Contains(CurrentWeaponType))
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing attacks for weapon type %s, reinitializing..."),
				*UEnum::GetValueAsString(CurrentWeaponType));
			continue;
		}

		TArray<FElemental_Struct>& Attacks = WeaponElementalAttacks[CurrentWeaponType].ElementalAttacks;

		// Verify Fire attacks match proficiency
		bool hasLevel2Fire = false;
		bool hasLevel3Fire = false;

		for (const FElemental_Struct& Attack : Attacks)
		{
			if (Attack.ElementalType == EElementalAttackType::Fire)
			{
				if (Attack.ElementalLevel == 2) hasLevel2Fire = true;
				if (Attack.ElementalLevel == 3) hasLevel3Fire = true;
			}
		}

		// Log any mismatches
		if (ProficiencyStruct.FireLevel >= 2 && !hasLevel2Fire)
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing Level 2 Fire attack for %s despite having required proficiency"),
				*UEnum::GetValueAsString(CurrentWeaponType));
		}

		// Similar checks for Ice and Thunder...
	}

}




void ARen_Low_Poly_Character::LogCurrentELementalAttacks()
{

	// Log total number of weapons with attacks
	UE_LOG(LogTemp, Warning, TEXT("=== Current Elemental Attacks Status ==="));
	UE_LOG(LogTemp, Warning, TEXT("Total weapon types with attacks: %d"), WeaponElementalAttacks.Num());

	// For each weapon type
	for (const auto& WeaponPair : WeaponElementalAttacks)
	{
		EWeaponType CurrentWeaponType = WeaponPair.Key;
		const FWeaponElementalAttacks& Attacks = WeaponPair.Value;

		UE_LOG(LogTemp, Warning, TEXT("\nWeapon Type: %s"), *UEnum::GetValueAsString(CurrentWeaponType));
		UE_LOG(LogTemp, Warning, TEXT("Total attacks: %d"), Attacks.ElementalAttacks.Num());

		// Count attacks by element and level
		int32 FireAttacks = 0;
		int32 IceAttacks = 0;
		int32 ThunderAttacks = 0;

		// Log each attack's details
		for (const FElemental_Struct& Attack : Attacks.ElementalAttacks)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - Attack: %s (Level %d)"), *Attack.ElementalAttackName, Attack.ElementalLevel);

			switch (Attack.ElementalType)
			{
			case EElementalAttackType::Fire:
				FireAttacks++;
				break;
			case EElementalAttackType::Ice:
				IceAttacks++;
				break;
			case EElementalAttackType::Thunder:
				ThunderAttacks++;
				break;
			}
		}

		// Log summary for this weapon
		UE_LOG(LogTemp, Warning, TEXT("Summary for %s:"), *UEnum::GetValueAsString(CurrentWeaponType));
		UE_LOG(LogTemp, Warning, TEXT("  Fire Attacks: %d"), FireAttacks);
		UE_LOG(LogTemp, Warning, TEXT("  Ice Attacks: %d"), IceAttacks);
		UE_LOG(LogTemp, Warning, TEXT("  Thunder Attacks: %d"), ThunderAttacks);

		// Also log the current proficiency levels for comparison
		if (WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(CurrentWeaponType))
		{
			const FElemental_Proficiency_Struct& Proficiency = WeaponElementalProficiency.ElementalWeaponProficiencyMap[CurrentWeaponType];
			UE_LOG(LogTemp, Warning, TEXT("Current Proficiency Levels:"));
			UE_LOG(LogTemp, Warning, TEXT("  Fire Level: %d"), Proficiency.FireLevel);
			UE_LOG(LogTemp, Warning, TEXT("  Ice Level: %d"), Proficiency.IceLevel);
			UE_LOG(LogTemp, Warning, TEXT("  Thunder Level: %d"), Proficiency.ThunderLevel);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("=== End of Elemental Attacks Status ===\n"));



}






void ARen_Low_Poly_Character::ApplyPowerUp(ESpecialPowerUp PowerUp)
{

	CurrentPowerUp = PowerUp; // Set the current power-up

	switch (PowerUp)

	{
	case ESpecialPowerUp::Berserk:
		UE_LOG(LogTemp, Warning, TEXT("Berserk Activated: Increased Attack Damage!"));

		BaseAttack *= 15.0f;
		
		GetWorld()->GetTimerManager().SetTimer(ResetAttackTimer, this, &ARen_Low_Poly_Character::ResetAttackPower, 35.0f, false);

		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("BERSERK!"));

		break;




	case ESpecialPowerUp::Invulnerability:
		UE_LOG(LogTemp, Warning, TEXT("Invulnerability Activated: Cannot Take Damage!"));

		GetMesh()->SetGenerateOverlapEvents(false);
		GetWorld()->GetTimerManager().SetTimer(InvulnerabilityTimer, this, &ARen_Low_Poly_Character::NullifyInvulnerability, 35.0f, false);
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("INVULNERABLE!"));

		break;




	case ESpecialPowerUp::HealthRegen:
		UE_LOG(LogTemp, Warning, TEXT("Health Regen Activated: Regenerating Health!"));
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("REGEN HEALTH"));

		{
			
			
			GetWorld()->GetTimerManager().SetTimer(RegenHealthTimer, this, &ARen_Low_Poly_Character::RegenHealth, 3.0f, true); // Trigger every 3 seconds
			GetWorld()->GetTimerManager().SetTimer(RegenHealthDurationTimer, this, &ARen_Low_Poly_Character::CancelHealthRegen, 35.0f, false); // Stop after 15 seconds

			break;

		}



	case ESpecialPowerUp::DoublePoints:


		UE_LOG(LogTemp, Warning, TEXT("Double Points Activated: Score Multiplier!"));
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("Double Points"));



		bDoublePoints = true;

		GetWorld()->GetTimerManager().SetTimer(InvulnerabilityTimer, this, &ARen_Low_Poly_Character::CancelDoublePoints, 30.0f, false);

		break;





	default:
		UE_LOG(LogTemp, Warning, TEXT("No Power-Up Activated."));
		break;
	}


}




void ARen_Low_Poly_Character::ResetAttackPower()
{


	BaseAttack = InitialAttack;
	GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Red, TEXT("Berserk Finished"));
	UE_LOG(LogTemp, Warning, TEXT("Berserk Ended: Attack Power Reset to Initial Value (%f)."), InitialAttack);


}


void ARen_Low_Poly_Character::NullifyInvulnerability()
{

	
	GetMesh()->SetGenerateOverlapEvents(true);
	GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Red, TEXT("Invulnerability Finished"));


}



void ARen_Low_Poly_Character::RegenHealth()
{

	const float RegenHealth = HealthStruct.CurrentHealth * 0.05f; //add 5% of health to character
	HealthStruct.CurrentHealth = FMath::Clamp(HealthStruct.CurrentHealth + RegenHealth, 0.0f, HealthStruct.MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Healing Aura: Health Regenerated to %f"), HealthStruct.CurrentHealth);
	GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Red, FString::Printf(TEXT("Healing Aura: Health Regenerated to %f"), HealthStruct.CurrentHealth));


}



void ARen_Low_Poly_Character::CancelHealthRegen()
{

	GetWorld()->GetTimerManager().ClearTimer(RegenHealthTimer); // Stop periodic regeneration

	UE_LOG(LogTemp, Warning, TEXT("Healing Aura Ended"));

	GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Red, TEXT("Regen Finished"));


}



void ARen_Low_Poly_Character::CancelDoublePoints()
{


	bDoublePoints = false;
	GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor::Red, TEXT("Double Points Finished"));


}

void ARen_Low_Poly_Character::DecreaseHealth(int amount)
{

	HealthStruct.CurrentHealth -= amount;

}













void ARen_Low_Poly_Character::TriggerCameraShake()
{


	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);


	if (PlayerController)

	{

		PlayerController->ClientStartCameraShake(MyCameraShakeClass, 1.0f);

	}



}




void ARen_Low_Poly_Character::GenerateStatUpgradeMessages()
{
	TArray<FString> StatMessages;


	// Check and format the attack power change
	if (BaseAttack > InitialAttack)
	{
		StatMessages.Add(FString::Printf(TEXT("Attack %d -> %d"), static_cast<int32>(InitialAttack), static_cast<int32>(BaseAttack)));

	}

	// Check and format the defense change
	if (BaseDefence > InitialDefense)
	{
		StatMessages.Add(FString::Printf(TEXT("Defense %d -> %d"), static_cast<int32>(InitialDefense), static_cast<int32>(BaseDefence)));
	}

	// Check and format the elemental power change
	if (BaseElementalAttack > InitialElemental)
	{
		StatMessages.Add(FString::Printf(TEXT("Elemental %d -> %d"), static_cast<int32>(InitialElemental), static_cast<int32>(BaseElementalAttack)));
	}

	// Check and format the max health change
	if (HealthStruct.MaxHealth > InitialMaxHealth)
	{
		StatMessages.Add(FString::Printf(TEXT("Health %d -> %d"), static_cast<int32>(InitialMaxHealth), static_cast<int32>(HealthStruct.MaxHealth)));
	}

	if (ManaStruct.MaxMana > InitialMaxMana)
	{
		StatMessages.Add(FString::Printf(TEXT("Mana %d -> %d"), static_cast<int32>(InitialMaxMana), static_cast<int32>(ManaStruct.MaxMana)));
	}
	
	for (const FString& TechniqueName : QueuedUnlockTechniques)

	{

		StatMessages.Add(FString::Printf(TEXT("Unlocked: %s"), *TechniqueName));

	}


	// Update the previous stat values
	PreviousAttackPower = BaseAttack;
	PreviousDefense = BaseDefence;
	PreviousElementalPower = BaseElementalAttack;
	PreviousMaxHealth = HealthStruct.MaxHealth;
	PreviousMaxMana = ManaStruct.MaxMana;

	// Send these messages to your UI
	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->ShowStatsUpgradeNotification(StatMessages);
	}
}





void ARen_Low_Poly_Character::AddWeaponEXP(float ExpAmount)
{


	// Check if the currently equipped weapon exists in the proficiency map
	if (WeaponProficiencyMap.Contains(WeaponType))
	{
		FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[WeaponType];

		// Get current level's threshold
		float CurrentThreshold = 100.0f; // Default threshold
		if (Proficiency.WeaponProficiencyThresholds.Contains(Proficiency.WeaponLevel))
		{
			CurrentThreshold = Proficiency.WeaponProficiencyThresholds[Proficiency.WeaponLevel];
		}

		// Add EXP
		Proficiency.CurrentEXP += ExpAmount;

		UE_LOG(LogTemp, Warning, TEXT("Added %.2f EXP to %s"),
			ExpAmount,
			*UEnum::GetValueAsString(WeaponType));
		UE_LOG(LogTemp, Warning, TEXT("Current EXP: %.2f / %.2f"),
			Proficiency.CurrentEXP,
			CurrentThreshold);
		UE_LOG(LogTemp, Warning, TEXT("Current Level: %d"),
			Proficiency.WeaponLevel);

		// Immediately check for level up
		CheckAndApplyWeaponLevelUp(WeaponType);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot add EXP: WeaponProficiencyMap does not contain weapon type: %s"),
			*UEnum::GetValueAsString(WeaponType));
	}

}




void ARen_Low_Poly_Character::CheckAndApplyWeaponLevelUp(EWeaponType TheWeaponType)
{

	if (!WeaponProficiencyMap.Contains(WeaponType))
	{
		return;
	}

	FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[WeaponType];

	// Get current level's threshold
	float CurrentThreshold = 100.0f; // Default threshold
	if (Proficiency.WeaponProficiencyThresholds.Contains(Proficiency.WeaponLevel))
	{
		CurrentThreshold = Proficiency.WeaponProficiencyThresholds[Proficiency.WeaponLevel];
	}

	// Check if we have enough EXP to level up and haven't hit max level
	while (Proficiency.CurrentEXP >= CurrentThreshold && Proficiency.WeaponLevel < 30)
	{
		// Level up!
		Proficiency.WeaponLevel++;

		// Subtract the EXP used for this level
		Proficiency.CurrentEXP -= CurrentThreshold;

		// Update stats based on new level
		if (Proficiency.AttackBoostPerLevel.Contains(Proficiency.WeaponLevel))
		{
			Proficiency.AttackPowerBoost = Proficiency.AttackBoostPerLevel[Proficiency.WeaponLevel];
		}
		else
		{
			Proficiency.AttackPowerBoost += 4.0f;
		}

		if (Proficiency.DefenseBoostPerLevel.Contains(Proficiency.WeaponLevel))
		{
			Proficiency.DefenseBoost = Proficiency.DefenseBoostPerLevel[Proficiency.WeaponLevel];
		}
		else
		{
			Proficiency.DefenseBoost += 2.0f;
		}

		if (Proficiency.ElementalBoostPerLevel.Contains(Proficiency.WeaponLevel))
		{
			Proficiency.ElementalPowerBoost = Proficiency.ElementalBoostPerLevel[Proficiency.WeaponLevel];
		}
		else
		{
			Proficiency.ElementalPowerBoost += 3.0f;
		}

		if (Proficiency.HealthBoostPerLevel.Contains(Proficiency.WeaponLevel))
		{
			Proficiency.MaxHealthBoost = Proficiency.HealthBoostPerLevel[Proficiency.WeaponLevel];
		}
		else
		{
			Proficiency.MaxHealthBoost += 10.0f;
		}

		if (Proficiency.ManaBoostPerLevel.Contains(Proficiency.WeaponLevel))
		{
			Proficiency.MaxManaBoost = Proficiency.ManaBoostPerLevel[Proficiency.WeaponLevel];
		}
		else
		{
			Proficiency.MaxManaBoost += 15.0f;
		}

		// Get next level's threshold
		if (Proficiency.WeaponProficiencyThresholds.Contains(Proficiency.WeaponLevel))
		{
			CurrentThreshold = Proficiency.WeaponProficiencyThresholds[Proficiency.WeaponLevel];
		}
		else
		{
			CurrentThreshold *= 1.25f;
		}

		// Update total stats
		UpdateStatsBasedOnWeapon();

		// Check for and unlock any new techniques
		UnlockWeaponTechnique(WeaponType, Proficiency.WeaponLevel);

		// Log level up and compare with initial stats
		UE_LOG(LogTemp, Warning, TEXT("=== WEAPON LEVEL UP ==="));
		UE_LOG(LogTemp, Warning, TEXT("%s reached level %d!"),
			*UEnum::GetValueAsString(WeaponType),
			Proficiency.WeaponLevel);
		UE_LOG(LogTemp, Warning, TEXT("Attack: %.2f -> %.2f (Initial: %.2f)"),
			InitialAttack, BaseAttack, InitialAttack);
		UE_LOG(LogTemp, Warning, TEXT("Defense: %.2f -> %.2f (Initial: %.2f)"),
			InitialDefense, BaseDefence, InitialDefense);
		UE_LOG(LogTemp, Warning, TEXT("Elemental: %.2f -> %.2f (Initial: %.2f)"),
			InitialElemental, BaseElementalAttack, InitialElemental);
		UE_LOG(LogTemp, Warning, TEXT("Max Health: %.2f -> %.2f (Initial: %.2f)"),
			InitialMaxHealth, HealthStruct.MaxHealth, InitialMaxHealth);
		UE_LOG(LogTemp, Warning, TEXT("Max Mana: %.2f -> %.2f (Initial: %.2f)"),
			InitialMaxMana, ManaStruct.MaxMana, InitialMaxMana);
		UE_LOG(LogTemp, Warning, TEXT("Next level requires: %.2f EXP"), CurrentThreshold);
	}
}



void ARen_Low_Poly_Character::UpdateEnemyArrows()
{

	// Loop through the enemies in the map and update the arrow widgets
	for (auto It = EnemyArrowMap.CreateIterator(); It; ++It)
	{
		AEnemy_Poly* Enemy = It.Key();
		UEnemy_Detection_Arrow* ArrowWidget = It.Value();

		if (!Enemy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is null in EnemyArrowMap! Removing entry."));
			It.RemoveCurrent();  // Remove the entry from the map if the enemy is null
			continue;  // Skip to the next iteration
		}

		if (!ArrowWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("ArrowWidget is null for enemy %s! Removing entry."), *Enemy->GetName());
			It.RemoveCurrent();  // Remove the entry from the map if the arrow widget is null
			continue;  // Skip to the next iteration
		}

		// Ensure the enemy is not marked for destruction (pending kill)
		if (Enemy->IsPendingKillPending())
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s is pending kill, removing entry from map."), *Enemy->GetName());
			It.RemoveCurrent();  // Remove the entry from the map if the enemy is pending kill
			continue;  // Skip to the next iteration
		}

		// Call the function to check the position and update the arrow widget
		CheckAndDisplayArrow(Enemy, ArrowWidget);
	}

}



void ARen_Low_Poly_Character::CheckAndDisplayArrow(AActor* Enemy, UEnemy_Detection_Arrow* ArrowWidget)
{
	// Check if ArrowWidget is valid
	if (!ArrowWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("ArrowWidget is null!"));
		return;
	}

	// Check if Enemy is valid
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy is null!"));
		return;
	}

	// Safely cast the enemy to AEnemy_Poly to access its properties
	AEnemy_Poly* EnemyPoly = Cast<AEnemy_Poly>(Enemy);
	if (!EnemyPoly)
	{
		// Log the class name of the enemy to identify why the cast failed
		UE_LOG(LogTemp, Warning, TEXT("Enemy is not of type AEnemy_Poly! Actual class: %s"), *Enemy->GetClass()->GetName());
		return;
	}

	// Get the screen position of the enemy
	FVector2D ScreenPosition;
	bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Enemy->GetActorLocation(), ScreenPosition);

	// Get the viewport size
	int32 ViewportWidth, ViewportHeight;
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewportWidth, ViewportHeight);
	FVector2D ViewportSize(ViewportWidth, ViewportHeight);

	// Check if the enemy is off-screen
	bool bOffScreen = ScreenPosition.X < 0 || ScreenPosition.X > ViewportSize.X || ScreenPosition.Y < 0 || ScreenPosition.Y > ViewportSize.Y;

	// If the enemy is off-screen, show the arrow
	if (bOffScreen)
	{
		ArrowWidget->SetVisibility(ESlateVisibility::Visible);

		// Calculate the rotation of the arrow to point towards the enemy
		FVector PlayerLocation = GetActorLocation();
		FVector EnemyLocation = EnemyPoly->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, EnemyLocation);

		// Calculate the direction from the center of the screen to the enemy
		FVector2D Direction = ScreenPosition - FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
		Direction.Normalize();

		FVector2D EdgePosition = FVector2D(0, 0);  // Position along the edge
		float EdgePadding = 10.0f;

		// Determine if the arrow should be on the top/bottom or left/right
		if (FMath::Abs(Direction.X) > FMath::Abs(Direction.Y))
		{
			// Place on the left or right edge
			if (Direction.X < 0)
			{
				EdgePosition.X = EdgePadding;  // Left edge
			}
			else
			{
				EdgePosition.X = ViewportSize.X - EdgePadding;  // Right edge
			}

			EdgePosition.Y = ViewportSize.Y / 2 + (Direction.Y * (ViewportSize.Y / 2 - EdgePadding)); // Center vertically along the edge
		}
		else
		{
			// Place on the top or bottom edge
			if (Direction.Y < 0)
			{
				EdgePosition.Y = EdgePadding;  // Top edge
			}
			else
			{
				EdgePosition.Y = ViewportSize.Y - EdgePadding;  // Bottom edge
			}
			EdgePosition.X = ViewportSize.X / 2 + (Direction.X * (ViewportSize.X / 2 - EdgePadding)); // Center horizontally along the edge
		}

		// Set the arrow widget position to the calculated edge position
		ArrowWidget->SetPositionInViewport(EdgePosition);

		// Update the rotation of the arrow widget to face the enemy
		ArrowWidget->UpdateArrowRotation(LookAtRotation.Yaw);
	}
	else
	{
		// If the enemy is on-screen, hide the arrow
		ArrowWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}









// Called when the game starts or when spawned
void ARen_Low_Poly_Character::BeginPlay()
{
	Super::BeginPlay();

	// Cast to ALowPoly_Survival_GameMode
	LowPoly_Survival_GameMode = Cast<ALowPoly_Survival_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (LowPoly_Survival_GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Mode successfully cast to ALowPoly_Survival_GameMode"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast to ALowPoly_Survival_GameMode. Check the level's game mode settings."));
	}


	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC)

	{
		FInputModeGameOnly GameOnlyInput;

		PC->SetInputMode(GameOnlyInput);

	}

	// Initialize Elemental Attacks
	//InitialiseElementalAttacks();

	// Initialize Elemental Proficiencies
	//InitialiseElementalProficiencies();


	LoadHighScore();
	LoadPlayerProgress();
	EnsureAllInitialisation();


	FindResultsCamera();

	LogCurrentELementalAttacks();


	AbilityStruct.InitializeAbilityPoints();

	HealthStruct.InitializeHealth();
	HealthStruct.CurrentHealth = HealthStruct.MaxHealth;

	ManaStruct.InitializeMana();
	ManaStruct.CurrentMana = ManaStruct.MaxMana;

	//TechniqueStruct.CurrentGauge = 100.0f;
	TechniqueStruct.MaxGauge = 100.0f;
	TechniqueStruct.TechniquePoints = 5;
	TechniqueStruct.MaxTechniquePoints = 7;


	AbilityStruct.CurrentAbilityPoints = 0.0f;



	// Log Fire proficiency thresholds to confirm initialization
	for (const auto& Pair : ElementalProficiency.FireProficiencyThresholds)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire Threshold: Level %d -> %f EXP"), Pair.Key, Pair.Value);
	}


	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UGame_Instance* CustomGameInstance = Cast<UGame_Instance>(GameInstance);
		if (CustomGameInstance)
		{

			WeaponType = CustomGameInstance->SelectedWeapon;


		}

	}





	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		LockOnCandidates.Add(Actor);
	}




	// Ensure WeaponProficiencyMap has entries for all weapon types, even if not loaded
	if (!WeaponProficiencyMap.Contains(EWeaponType::Sword))
	{
		WeaponProficiencyMap.Add(EWeaponType::Sword, FWeapon_Proficiency_Struct());
	}
	if (!WeaponProficiencyMap.Contains(EWeaponType::Staff))
	{
		WeaponProficiencyMap.Add(EWeaponType::Staff, FWeapon_Proficiency_Struct());
	}



	// Initialize any stats if necessary
	if (WeaponType == EWeaponType::Sword)
	{
		// Set initial values (or defaults) here if necessary
		BaseAttack = 10.0f;
		BaseDefence = 2.0f;
		BaseElementalAttack = 4.0f;
		HealthStruct.MaxHealth = 140.0f;
		ManaStruct.MaxMana = 60.0f;
	}
	else if (WeaponType == EWeaponType::Staff)
	{
		BaseAttack = 3.0f;
		BaseDefence = 2.0f;
		BaseElementalAttack = 4.0f;
		HealthStruct.MaxHealth = 130.0f;
		ManaStruct.MaxMana = 95.0f;
	}

	// Then, update the stats based on the weapon proficiency
	UpdateStatsBasedOnWeapon();


	// Store initial stats
	InitialAttack = BaseAttack;
	InitialDefense = BaseDefence;
	InitialElemental = BaseElementalAttack;
	InitialMaxHealth = HealthStruct.MaxHealth;
	InitialMaxMana = ManaStruct.MaxMana;

	UE_LOG(LogTemp, Log, TEXT("Initial Stats - Attack: %f, Defense: %f, Elemental: %f, MaxHealth: %f, MaxMana: %f"),
		InitialAttack, InitialDefense, InitialElemental, InitialMaxHealth, InitialMaxMana);


	// Create and populate Sword techniques
	FWeaponTechniqueMap SwordTechniquesForMap;
	SwordTechniquesForMap.LevelToTechnique.Add(6, TEXT("Voltage Breaker"));
	SwordTechniquesForMap.LevelToTechnique.Add(10, TEXT("Tempest Barrage"));
	SwordTechniquesForMap.LevelToTechnique.Add(19, TEXT("Static Rush"));

	// Add Sword techniques to the main map
	WeaponLevelToTechniqueMap.Add(EWeaponType::Sword, SwordTechniquesForMap);



	// Create and populate Staff techniques
	FWeaponTechniqueMap StaffTechniquesForMap;
	StaffTechniquesForMap.LevelToTechnique.Add(4, TEXT("Stone Rush"));

	// Add Staff techniques to the main map
	WeaponLevelToTechniqueMap.Add(EWeaponType::Staff, StaffTechniquesForMap);






	if (!WeaponTechniques.Contains(WeaponType))  // Only initialize if no saved data
	{
		// Initialize Sword techniques in the array
		Techniques.Add(FTechnique_Struct{ TEXT("Stormstrike Flurry"), TEXT("Furious multi-strike sword combo."), true, StormStrikeFlurryAnimMontage, 1.6f, 1, 1 });
		Techniques.Add(FTechnique_Struct{ TEXT("Voltage Breaker"), TEXT("Electrifying ground-slam force field."), false, VoltageBreakerAnimMontage, 1.3f, 2, 2 });
		Techniques.Add(FTechnique_Struct{ TEXT("Tempest Barrage"), TEXT("Rapid flurry of strikes."), false, TempestBarrageAnimMontage, 1.7f, 3, 3 });
		Techniques.Add(FTechnique_Struct{ TEXT("Tempest Barrage"), TEXT("Lightning-infused sword combo."), false, StaticRushAnimMontage, 2.4f, 4, 4 });

		// Create FWeaponTechniques struct and store the techniques
		FWeaponTechniques SwordTechniques;
		SwordTechniques.WeaponTechniques = Techniques;

		// Add to the map
		WeaponTechniques.Add(WeaponType, SwordTechniques);
	}
	else
	{
		// If we have saved data, use it to populate the Techniques array
		Techniques = WeaponTechniques[WeaponType].WeaponTechniques;
	}

	WeaponElementalAttacks.Add(EWeaponType::Sword, FWeaponElementalAttacks{
		{
			FElemental_Struct(TEXT("Fire"), EElementalAttackType::Fire, 1.7f, 15.0f, 1, true, FireProjectileAnimation, TEXT("Burns enemies over time.")),
			FElemental_Struct(TEXT("Ice"), EElementalAttackType::Ice, 1.9f, 15.0f, 1, true, IceProjectileAnimation, TEXT("Freezes enemies over time.")),
			FElemental_Struct(TEXT("Thunder"), EElementalAttackType::Thunder, 1.5f, 10.0f, 1, true, ThunderProjectileAnimation, TEXT("Stuns enemies over time.")),
			FElemental_Struct(TEXT("Fire Lv.2"), EElementalAttackType::Fire, 2.9f, 25.0f, 2, false, FireAOEAnimation, TEXT("Creates an explosion, burns enemies for longer")),
			FElemental_Struct(TEXT("Ice Lv.2"), EElementalAttackType::Ice, 2.5f, 30.0f, 2, false, IceAOEAnimation, TEXT("Summons ice shards, freezing enemies for longer.")),
			FElemental_Struct(TEXT("Thunder Lv.2"), EElementalAttackType::Thunder, 1.9f, 15.0f, 2, false, ThunderAOEAnimation, TEXT("Summons lightning, stunning enemies for longer.")),
			FElemental_Struct(TEXT("Fire Lv.3"), EElementalAttackType::Fire, 1.5f, 30.0f, 3, false, FireGroundAnimation, TEXT("Summons molten spikes, burns enemies for an extended time.")),
			FElemental_Struct(TEXT("Ice Lv.3"), EElementalAttackType::Ice, 1.9f, 35.0f, 3, false, IceGroundAnimation, TEXT("Summons ice spiral, freezing enemies for an extended time.")),
			FElemental_Struct(TEXT("Thunder Lv.3"), EElementalAttackType::Thunder, 1.5f, 20.0f, 3, false, ThunderGroundAnimation, TEXT("Summons ice spiral, freezing enemies for an extended time."))
		}
		});
	





	if (WeaponType == EWeaponType::Staff)
	{
		// Initialize Staff techniques
		Techniques.Add(FTechnique_Struct{ TEXT("Meteor Strike"), TEXT("Fiery meteor devastates nearby enemies."), true, MeteorStrikeAnimMontage, 3.5f, 1, 1 });
		Techniques.Add(FTechnique_Struct{ TEXT("Frost Rain"), TEXT("Icicles rain down, freezing foes."), false, FrostRainAnimMontage, 3.1f, 2, 2 });
		Techniques.Add(FTechnique_Struct{ TEXT("Feud Fang"), TEXT("Dark spikes pierce from below."), false, FeudFangAnimMontage, 3.7f, 3, 3 });



		WeaponElementalAttacks.Add(EWeaponType::Staff, FWeaponElementalAttacks{
	   {FElemental_Struct(TEXT("Fire"), EElementalAttackType::Fire, 2.7f, 15.0f, 1, true, FireProjectileAnimation, TEXT("Burns enemies over time.")),
		FElemental_Struct(TEXT("Ice"), EElementalAttackType::Ice, 1.9f, 15.0f, 1, true, IceProjectileAnimation, TEXT("Freezes enemies over time.")),
		FElemental_Struct(TEXT("Thunder"), EElementalAttackType::Thunder, 1.5f, 10.0f, 1, true, ThunderProjectileAnimation, TEXT("Stuns enemies over time.")),
		FElemental_Struct(TEXT("Fire Lv.2"), EElementalAttackType::Fire, 2.9f, 25.0f, 2, false, FireAOEAnimation, TEXT("Creates an explosion, burns enemies for longer")),
		FElemental_Struct(TEXT("Ice Lv.2"), EElementalAttackType::Ice, 2.5f, 30.0f, 2, false, IceAOEAnimation, TEXT("Summons ice shards, freezing enemies for longer.")),
		FElemental_Struct(TEXT("Thunder Lv.2"), EElementalAttackType::Thunder, 1.9f, 15.0f, 2, false, ThunderAOEAnimation, TEXT("Summons lightning, stunning enemies for longer.")),
		FElemental_Struct(TEXT("Fire Lv.3"), EElementalAttackType::Fire, 1.5f, 30.0f, 3, false, FireGroundAnimation, TEXT("Summons molten spikes, burns enemies for an extended time.")),
		FElemental_Struct(TEXT("Ice Lv.3"), EElementalAttackType::Ice, 1.9f, 35.0f, 3, false, IceGroundAnimation, TEXT("Summons ice spiral, freezing enemies for an extended time.")),
		FElemental_Struct(TEXT("Thunder Lv.3"), EElementalAttackType::Thunder, 1.5f, 20.0f, 3, false, ThunderGroundAnimation, TEXT("Summons ice spiral, freezing enemies for an extended time."))

		}

			});
	

	}

	

	// Create the command menu widget
	if (CommandMenuWidgetClass)
	{
		CommandMenuWidget = CreateWidget<UCommand_Menu_Widget>(GetWorld(), CommandMenuWidgetClass);
		if (CommandMenuWidget)
		{
			CommandMenuWidget->AddToViewport(); // Add it to the player's viewport
		}
	}


	
	FVector2D ViewportSizee;

	// Get the viewport size once at the start
	GEngine->GameViewport->GetViewportSize(ViewportSizee);

	// Find all enemies of class AEnemy_Poly in the world
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy_Poly::StaticClass(), FoundEnemies);

	// Log the number of enemies found
	UE_LOG(LogTemp, Warning, TEXT("Number of enemies found: %d"), FoundEnemies.Num());

	// Loop through each found enemy and create a detection arrow for each
	for (AActor* EnemyActor : FoundEnemies)
	{
		// Cast the actor to AEnemy_Poly
		AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(EnemyActor);
		if (Enemy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy found at location: %s"), *Enemy->GetActorLocation().ToString());

			if (EnemyArrowWidgetClass)
			{
				// Create the arrow widget for the enemy
				UEnemy_Detection_Arrow* NewArrowWidget = CreateWidget<UEnemy_Detection_Arrow>(GetWorld(), EnemyArrowWidgetClass);
				if (NewArrowWidget)
				{
					// Add to the viewport
					NewArrowWidget->AddToViewport();

					// Store the widget in the map with the enemy as the key
					EnemyArrowMap.Add(Enemy, NewArrowWidget);

					UE_LOG(LogTemp, Warning, TEXT("Created and added Enemy Detection Arrow widget to viewport."));

					// Check if the widget is visible and log the result
					if (NewArrowWidget->IsVisible())
					{
						UE_LOG(LogTemp, Warning, TEXT("Enemy arrow widget is visible on screen."));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Enemy arrow widget is NOT visible on screen."));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to create Enemy Detection Arrow widget."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("EnemyArrowWidgetClass is null. Ensure it is set in the character class properties."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to cast actor to AEnemy_Poly."));
		}
	}


	TechniqueAvailability.Init(false, Techniques.Num());

	
	// Bind the input action
	InputComponent->BindAction("Open Commands Menu", IE_Pressed, this, &ARen_Low_Poly_Character::ToggleCommandMenu);
	InputComponent->BindAction("Roll Dodge or Back", IE_Pressed, this, &ARen_Low_Poly_Character::HandleBackInput);



	if (WeaponProficiencyMap.Contains(EWeaponType::Sword))
	{
		FWeapon_Proficiency_Struct& SwordProficiency = WeaponProficiencyMap[EWeaponType::Sword];

		// Get current level's threshold for Sword
		float SwordThreshold = 100.0f; // Default threshold
		if (SwordProficiency.WeaponProficiencyThresholds.Contains(SwordProficiency.WeaponLevel))
		{
			SwordThreshold = SwordProficiency.WeaponProficiencyThresholds[SwordProficiency.WeaponLevel];
		}

		// Log weapon stats
		UE_LOG(LogTemp, Log, TEXT("Sword Weapon Stats:"));
		UE_LOG(LogTemp, Log, TEXT("Level: %d"), SwordProficiency.WeaponLevel);
		UE_LOG(LogTemp, Log, TEXT("Current EXP: %.2f / %.2f"), SwordProficiency.CurrentEXP, SwordThreshold);
		UE_LOG(LogTemp, Log, TEXT("Attack Power Boost: %.2f"), SwordProficiency.AttackPowerBoost);
		UE_LOG(LogTemp, Log, TEXT("Defense Boost: %.2f"), SwordProficiency.DefenseBoost);
		UE_LOG(LogTemp, Log, TEXT("Elemental Power Boost: %.2f"), SwordProficiency.ElementalPowerBoost);
		UE_LOG(LogTemp, Log, TEXT("Max Health Boost: %.2f"), SwordProficiency.MaxHealthBoost);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sword proficiency not found!"));
	}

	if (WeaponProficiencyMap.Contains(EWeaponType::Staff))
	{
		FWeapon_Proficiency_Struct& StaffProficiency = WeaponProficiencyMap[EWeaponType::Staff];

		// Get current level's threshold for Staff
		float StaffThreshold = 100.0f; // Default threshold
		if (StaffProficiency.WeaponProficiencyThresholds.Contains(StaffProficiency.WeaponLevel))
		{
			StaffThreshold = StaffProficiency.WeaponProficiencyThresholds[StaffProficiency.WeaponLevel];
		}

		// Log weapon stats
		UE_LOG(LogTemp, Log, TEXT("Staff Weapon Stats:"));
		UE_LOG(LogTemp, Log, TEXT("Level: %d"), StaffProficiency.WeaponLevel);
		UE_LOG(LogTemp, Log, TEXT("Current EXP: %.2f / %.2f"), StaffProficiency.CurrentEXP, StaffThreshold);
		UE_LOG(LogTemp, Log, TEXT("Attack Power Boost: %.2f"), StaffProficiency.AttackPowerBoost);
		UE_LOG(LogTemp, Log, TEXT("Defense Boost: %.2f"), StaffProficiency.DefenseBoost);
		UE_LOG(LogTemp, Log, TEXT("Elemental Power Boost: %.2f"), StaffProficiency.ElementalPowerBoost);
		UE_LOG(LogTemp, Log, TEXT("Max Health Boost: %.2f"), StaffProficiency.MaxHealthBoost);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Staff proficiency not found!"));

	}

}
	






void ARen_Low_Poly_Character::SetItemsButtonFocus()
{


	if (CommandMenuWidget && CommandMenuWidget->ItemsButton && !bIsDead)
	{
		if (!bIsInventoryEmpty)
		{
			CommandMenuWidget->ItemsButton->SetKeyboardFocus(); // Focus on the Items Button
			UE_LOG(LogTemp, Warning, TEXT("Focus set on Items Button after delay."));
		}
		else
		{
			CommandMenuWidget->TechniquesButton->SetKeyboardFocus(); // Focus on the Techniques Button
			UE_LOG(LogTemp, Warning, TEXT("Focus set on Techniques Button because inventory is empty."));
		}
	}
}









void ARen_Low_Poly_Character::ToggleCommandMenu()
{
	if (CommandMenuWidget && CommandMenuWidget->WidgetSwitcher && !bIsDead)
	{
		int CurrentIndex = CommandMenuWidget->WidgetSwitcher->GetActiveWidgetIndex();

		// Check if we are currently at index 0 to open the command menu
		if (CurrentIndex == 0)
		{
			CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(1);
			CommandMenuWidget->PlayAnimationReverse(CommandMenuWidget->CommandMenuIcon_FadeAnim);
			CommandMenuWidget->PlayAnimation(CommandMenuWidget->CommandMenu_FadeAnim);
			UpdateVisibilityBasedOnIndex(1);  // Update visibility right after switching to index 1

			// Make sure buttons are visible
			CommandMenuWidget->ItemsButton->SetVisibility(ESlateVisibility::Visible);
			CommandMenuWidget->TechniquesButton->SetVisibility(ESlateVisibility::Visible);
			CommandMenuWidget->ElementalButton->SetVisibility(ESlateVisibility::Visible);


			// Add slight delay before setting keyboard focus to ensure UI updates
			GetWorldTimerManager().SetTimerForNextTick(this, &ARen_Low_Poly_Character::SetItemsButtonFocus);

			CommandMenuWidget->CheckInventoryAndSetFocus();

			SetInputModeForUI();
			bIsInUIMode = true;

			// Log for debugging
			UE_LOG(LogTemp, Warning, TEXT("Command Menu opened, index set to: %d"), CommandMenuWidget->WidgetSwitcher->GetActiveWidgetIndex());
		}
		else if (CurrentIndex == 1) // If already in the command menu
		{
			UpdateVisibilityBasedOnIndex(1);  // Update visibility for index 1
			if (CommandMenuWidget->ItemsButton)
			{
				CommandMenuWidget->ItemsButton->SetKeyboardFocus(); // Ensure focus remains on the Items Button
			}

			// Log for debugging
			UE_LOG(LogTemp, Warning, TEXT("Command Menu already open, focus set on Items Button."));
		}
		else if (CurrentIndex == 2) // If currently in the inventory
		{
			CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(1);
			UpdateVisibilityBasedOnIndex(1);  // Update visibility when switching back to command menu

			// Make sure buttons are visible when returning to command menu
			CommandMenuWidget->ItemsButton->SetVisibility(ESlateVisibility::Visible);
			CommandMenuWidget->TechniquesButton->SetVisibility(ESlateVisibility::Visible);
			CommandMenuWidget->ElementalButton->SetVisibility(ESlateVisibility::Visible);


			
			GetWorldTimerManager().SetTimerForNextTick(this, &ARen_Low_Poly_Character::SetItemsButtonFocus);
			

			bIsInventoryOpen = false;
			bIsTechniquesOpen = false;
			bIsElementalsOpen = false;
			// Log for debugging
			UE_LOG(LogTemp, Warning, TEXT("Returned to Command Menu from Inventory, index set to: %d"), CommandMenuWidget->WidgetSwitcher->GetActiveWidgetIndex());
		}
	}
}




void ARen_Low_Poly_Character::UpdateVisibilityBasedOnIndex(int Index)
{


	if (CommandMenuWidget)
	{
		if (Index == 1)
		{
			CommandMenuWidget->ItemsButton->SetVisibility(ESlateVisibility::Visible);
			CommandMenuWidget->TechniquesButton->SetVisibility(ESlateVisibility::Visible);
			CommandMenuWidget->ElementalButton->SetVisibility(ESlateVisibility::Visible);


			if (CommandMenuWidget->InventoryWidgetInstance && CommandMenuWidget->InventoryWidgetInstance->IsInViewport())
			{
				CommandMenuWidget->InventoryWidgetInstance->RemoveFromParent();
				CommandMenuWidget->ItemsButton->SetKeyboardFocus();

			}

			CommandMenuWidget->ItemsButton->SetKeyboardFocus();
		}
		else if (Index == 2)
		{
			CommandMenuWidget->ItemsButton->SetVisibility(ESlateVisibility::Collapsed);
			CommandMenuWidget->TechniquesButton->SetVisibility(ESlateVisibility::Collapsed);
			CommandMenuWidget->ElementalButton->SetVisibility(ESlateVisibility::Collapsed);


			if (CommandMenuWidget->InventoryWidgetInstance && !CommandMenuWidget->InventoryWidgetInstance->IsInViewport())
			{
				CommandMenuWidget->InventoryWidgetInstance->AddToViewport();
			}

			else if (Index == 3)

			{
				// Set techniques widget visibility and other required elements
				if (CommandMenuWidget->TechniquesWidgetInstance && !CommandMenuWidget->TechniquesWidgetInstance->IsInViewport())
				{
					CommandMenuWidget->TechniquesWidgetInstance->AddToViewport();
				}

				CommandMenuWidget->ItemsButton->SetVisibility(ESlateVisibility::Collapsed);
				CommandMenuWidget->TechniquesButton->SetVisibility(ESlateVisibility::Collapsed);
				CommandMenuWidget->ElementalButton->SetVisibility(ESlateVisibility::Collapsed);

			}

			else if (Index == 4)

			{
				// Set techniques widget visibility and other required elements
				if (CommandMenuWidget->ElementalAttacksWidgetInstance && !CommandMenuWidget->ElementalAttacksWidgetInstance->IsInViewport())
				{
					CommandMenuWidget->ElementalAttacksWidgetInstance->AddToViewport();
				}

				CommandMenuWidget->ItemsButton->SetVisibility(ESlateVisibility::Collapsed);
				CommandMenuWidget->TechniquesButton->SetVisibility(ESlateVisibility::Collapsed);
				CommandMenuWidget->ElementalButton->SetVisibility(ESlateVisibility::Collapsed);

			}
		}
	}


}





void ARen_Low_Poly_Character::OpenInventory()
{

	if (CommandMenuWidget && CommandMenuWidget->WidgetSwitcher)
	{
		// Set the WidgetSwitcher to display the inventory (index 2)
		CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(2);
		bIsInventoryOpen = true;
		bIsTechniquesOpen = false;
		bIsElementalsOpen = false;



		// Check and focus the inventory button after confirming it is initialized
		if (InventoryButtonWidget)
		{
			InventoryButtonWidget->SetKeyboardFocus();
		
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryButtonWidget is null, focus cannot be set."));
		}		SetInputModeForUI();

		bIsInUIMode = true;  // Track that we're still in UI mode
	}


}

void ARen_Low_Poly_Character::OpenTechniques()
{

	if (CommandMenuWidget && CommandMenuWidget->WidgetSwitcher)
	{
		// Set the WidgetSwitcher to display the techniques menu (index 3)
		CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(3);

		// Update visibility specifically for techniques menu
		UpdateVisibilityBasedOnIndex(3);
		bIsInventoryOpen = false;
		bIsElementalsOpen = false;
		bIsTechniquesOpen = true;

		LastFocusedButton = CommandMenuWidget->TechniquesButton;
	}


	SetInputModeForUI();
	bIsInUIMode = true; // Track UI mode for techniques menu

}




void ARen_Low_Poly_Character::OpenElementalAttacks()
{


	if (CommandMenuWidget && CommandMenuWidget->WidgetSwitcher)
	{
		// Set the WidgetSwitcher to display the elemental menu (index 4)
		CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(4);

		// Update visibility specifically for elemental menu
		UpdateVisibilityBasedOnIndex(4);
		bIsInventoryOpen = false;
		bIsTechniquesOpen = false;
		bIsElementalsOpen = true;
		LastFocusedButton = CommandMenuWidget->ElementalButton;

	}


	SetInputModeForUI();
	bIsInUIMode = true; // Track UI mode for elemental menu




}






void ARen_Low_Poly_Character::HandleBackInput()
{
	UE_LOG(LogTemp, Warning, TEXT("Back button pressed!"));

	if (CommandMenuWidget && CommandMenuWidget->WidgetSwitcher)
	{
		int CurrentIndex = CommandMenuWidget->WidgetSwitcher->GetActiveWidgetIndex();

		if (CurrentIndex == 1) // If in command menu
		{
			CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(0);
			CommandMenuWidget->PlayAnimation(CommandMenuWidget->CommandMenuIcon_FadeAnim);
			CommandMenuWidget->PlayAnimationReverse(CommandMenuWidget->CommandMenu_FadeAnim);
			SetInputModeForGameplay();
			bIsInUIMode = false; // Return to gameplay
		}
		else if (CurrentIndex == 2 || CurrentIndex == 3 || CurrentIndex == 4) // If in inventory, techniques, or elementals
		{
			CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(1);
			bIsInUIMode = true; // Still in UI mode

			// Explicitly restore focus to the correct button based on the last menu
			if (CurrentIndex == 2 && CommandMenuWidget->ItemsButton) // Coming from Inventory
			{
				CommandMenuWidget->ItemsButton->SetKeyboardFocus();
				UE_LOG(LogTemp, Warning, TEXT("Focus restored to Items Button."));
			}
			else if (CurrentIndex == 3 && CommandMenuWidget->TechniquesButton) // Coming from Techniques
			{
				CommandMenuWidget->TechniquesButton->SetKeyboardFocus();
				UE_LOG(LogTemp, Warning, TEXT("Focus restored to Techniques Button."));
			}
			else if (CurrentIndex == 4 && CommandMenuWidget->ElementalButton) // Coming from Elementals
			{
				CommandMenuWidget->ElementalButton->SetKeyboardFocus();
				UE_LOG(LogTemp, Warning, TEXT("Focus restored to Elemental Button."));
			}
			else if (LastFocusedButton) // Fallback to last focused button
			{
				LastFocusedButton->SetKeyboardFocus();
				UE_LOG(LogTemp, Warning, TEXT("Fallback: Focus restored to LastFocusedButton."));
			}
		}
	}
}




void ARen_Low_Poly_Character::SetInputModeForUI()
{

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(CommandMenuWidget->TakeWidget());  // Focus on the widget
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}

}





void ARen_Low_Poly_Character::SetInputModeForGameplay()
{

	// Set input mode back to game only, enable game input
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

}




void ARen_Low_Poly_Character::EnableUIInputWithGameInput()
{

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(CommandMenuWidget->TakeWidget());

		// This allows both UI and game input (for actions like back button)
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}

}




// Called every frame
void ARen_Low_Poly_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAbilityUsage();

	CheckGaugeMaximum();

	ToggleSoftLock();

	CheckTechniquePoints();

	CheckTechniquePointsMaximum();

	ControlTechniqueGaugeFill();

	ControlMPFill();

	// Call UpdateEnemyArrows only if the EnemyArrowMap is valid and not empty
	if (EnemyArrowMap.Num() > 0)
	{
		UpdateEnemyArrows();
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("EnemyArrowMap is empty! No arrows to update."));
	}

	

	FString StatsText = FString::Printf(TEXT("Current Attack: %.2f\nCurrent Defense: %.2f\nMax Health: %.2f\nCurrent Health: %.2f\nMax Mana: %f\nCurrent Mana: %f"),
		BaseAttack, BaseDefence, HealthStruct.MaxHealth, HealthStruct.CurrentHealth, ManaStruct.MaxMana, ManaStruct.CurrentMana);

	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, StatsText);


	

}




// Called to bind functionality to input
void ARen_Low_Poly_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARen_Low_Poly_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARen_Low_Poly_Character::MoveRight);


	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ARen_Low_Poly_Character::UseAbility);
	PlayerInputComponent->BindAction("Roll Dodge or Back", IE_Pressed, this, &ARen_Low_Poly_Character::HandleBackInput);

}