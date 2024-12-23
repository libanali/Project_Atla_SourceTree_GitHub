// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Low_Poly_Character.h"
#include "Kismet/Gameplaystatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy_Poly.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "Command_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Technique_Struct.h"
#include "Enemy_Detection_Arrow.h"
#include "Player_Save_Game.h"
#include "Game_Over_Widget.h"


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
	PowerUpCamera->SetupAttachment(GetMesh(), TEXT("spine_01")); // Use "HeadSocket" if your mesh has one

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


	//Weapon Proficiency
	QueuedUnlockTechniques = TArray<FString>();


	WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Sword, FElemental_Proficiency_Struct());
	WeaponElementalProficiency.ElementalWeaponProficiencyMap.Add(EWeaponType::Staff, FElemental_Proficiency_Struct());
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
		BaseAttack = 10.0f;
		BaseDefence = 2.0f;
		BaseElementalAttack = 4.0f;
		HealthStruct.MaxHealth = 140.0f;
		ManaStruct.MaxMana = 60.0f;
	}
	else if (WeaponType == EWeaponType::Staff)
	{
		BaseAttack = 77.0f;
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

	UE_LOG(LogTemp, Warning, TEXT("Updated Stats - Attack: %f, Defense: %f, Elemental: %f, Max Health: %f"),
		BaseAttack, BaseDefence, BaseElementalAttack, HealthStruct.MaxHealth);

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
		SaveGameInstance->SavedWeaponProficiencyMap = WeaponProficiencyMap;

		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player Save Slot"), 0))
		{
			UE_LOG(LogTemp, Log, TEXT("Weapon proficiency saved successfully."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to save weapon proficiency."));
		}
	}

}

void ARen_Low_Poly_Character::LoadPlayerProgress()
{

	UPlayer_Save_Game* LoadGameInstance = Cast<UPlayer_Save_Game>(UGameplayStatics::LoadGameFromSlot(TEXT("Player Save Slot"), 0));
	if (LoadGameInstance)
	{
		WeaponProficiencyMap = LoadGameInstance->SavedWeaponProficiencyMap;

		UE_LOG(LogTemp, Log, TEXT("Successfully loaded weapon proficiency map."));
		for (const TPair<EWeaponType, FWeapon_Proficiency_Struct>& Pair : WeaponProficiencyMap)
		{
			UE_LOG(LogTemp, Warning, TEXT("Loaded %s: Level %d, EXP %.2f"),
				*UEnum::GetValueAsString(Pair.Key),
				Pair.Value.WeaponLevel,
				Pair.Value.CurrentEXP);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No save game found. Initializing default values."));
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




void ARen_Low_Poly_Character::UnlockTechnique(FString TechniqueID)
{

	for (FTechnique_Struct& Technique : Techniques)
	{
		if (Technique.TechniqueName == TechniqueID && !Technique.bIsUnlocked)
		{
			Technique.bIsUnlocked = true;
			// Optionally, add any UI update or notification here to indicate the technique was unlocked
			UE_LOG(LogTemp, Log, TEXT("Unlocked Technique: %s"), *Technique.TechniqueName);
			break;
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




void ARen_Low_Poly_Character::UseElementalAttack(int32 ElementalIndex)
{
	if (ElementalIndex >= 0 && ElementalIndex < ElementalAttacks.Num())
	{
		FElemental_Struct& SelectedElementalAttack = ElementalAttacks[ElementalIndex];

		if (SelectedElementalAttack.bIsUnlocked && ManaStruct.CurrentMana >= SelectedElementalAttack.ManaCost)
		{
			// Deduct mana
			ManaStruct.CurrentMana -= SelectedElementalAttack.ManaCost;

			// Play the animation for the attack
			PlayAnimMontage(SelectedElementalAttack.Elemental_Attack_Animation);

			// Set the current elemental attack type
			CurrentElementalAttackType = SelectedElementalAttack.ElementalType;

		//	AddExperienceToElementalProfiency(WeaponType, SelectedElementalAttack.ElementalType, 250.0f);

			// Use the TimerHandle to add experience after a 1-second delay
			GetWorld()->GetTimerManager().SetTimer(
				Leveluphandle, // Timer handle
				FTimerDelegate::CreateUObject(
					this,
					&ARen_Low_Poly_Character::AddExperienceToElementalProfiency,
					WeaponType,                         // Pass WeaponType
					SelectedElementalAttack.ElementalType, // Pass ElementType
					250.0f                             // EXP Amount
				),
				1.0f, // Delay of 1 second
				false // Do not loop
			);

			// Log the successful use of the attack
			UE_LOG(LogTemp, Warning, TEXT("Used Elemental Attack: %s, Gained EXP!"),
				*SelectedElementalAttack.ElementalAttackName);

			// Log the successful use of the attack
			UE_LOG(LogTemp, Warning, TEXT("Used Elemental Attack: %s, Mana Cost: %f"),
				*SelectedElementalAttack.ElementalAttackName, SelectedElementalAttack.ManaCost);

		}
		else
		{
			// Log an error if the attack cannot be used
			UE_LOG(LogTemp, Warning, TEXT("Cannot use Elemental Attack %s: Not enough mana or locked!"),
				*SelectedElementalAttack.ElementalAttackName);
		}
	}
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





void ARen_Low_Poly_Character::AddExperienceToElementalProfiency(EWeaponType TheWeaponType, EElementalAttackType ElementType, float EXPAmount)
{


	// Check if the map contains the weapon type
	if (!WeaponElementalProficiency.ElementalWeaponProficiencyMap.Contains(TheWeaponType))
	{
		UE_LOG(LogTemp, Error, TEXT("ElementalWeaponProficiencyMap does not contain the weapon type: %s"),
			*UEnum::GetValueAsString(TheWeaponType));
		return;
	}



	FElemental_Proficiency_Struct* ProficiencyStruct = nullptr;

	// Find the proficiency map for the correct weapon
	if (TheWeaponType == EWeaponType::Sword)
	{
		ProficiencyStruct = &WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Sword];
	}
	else if (TheWeaponType == EWeaponType::Staff)
	{
		ProficiencyStruct = &WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Staff];
	}

	// Add EXP to the correct element's proficiency
	if (ProficiencyStruct)
	{
		switch (ElementType)
		{
		case EElementalAttackType::Fire:
			ProficiencyStruct->FireProficiency += EXPAmount;

			break;
		case EElementalAttackType::Ice:
			ProficiencyStruct->IceProficiency += EXPAmount;
			break;
		case EElementalAttackType::Thunder:
			ProficiencyStruct->ThunderProficiency += EXPAmount;
			break;
		default:
			break;
		}
	}

	// Check if the proficiency has leveled up
	CheckElementalLevelUp(TheWeaponType, ElementType);


}




void ARen_Low_Poly_Character::CheckElementalLevelUp(EWeaponType TheWeaponType, EElementalAttackType ElementType)
{

	FElemental_Proficiency_Struct* ProficiencyStruct = nullptr;

	if (TheWeaponType == EWeaponType::Sword)
	{
		ProficiencyStruct = &WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Sword];
	}
	else if (TheWeaponType == EWeaponType::Staff)
	{
		ProficiencyStruct = &WeaponElementalProficiency.ElementalWeaponProficiencyMap[EWeaponType::Staff];
	}

	if (ProficiencyStruct)
	{
		// Check Fire proficiency and level it up
		if (ElementType == EElementalAttackType::Fire)
		{
			if (ProficiencyStruct->FireProficiency >= 200.0f && ProficiencyStruct->FireLevel == 1) // Level 1 to 2
			{
				ProficiencyStruct->FireLevel = 2;
				ProficiencyStruct->FireProficiency = 0.0f;
				UnlockElementalAbilities(TheWeaponType, ElementType, 2);  // Unlock level 2 Fire abilities
			}
			else if (ProficiencyStruct->FireProficiency >= 300.0f && ProficiencyStruct->FireLevel == 2) // Level 2 to 3
			{
				ProficiencyStruct->FireLevel = 3;
				ProficiencyStruct->FireProficiency = 0.0f;
				UnlockElementalAbilities(TheWeaponType, ElementType, 3);  // Unlock level 3 Fire abilities
			}
		}

		// Repeat the same for Ice and Thunder
		else if (ElementType == EElementalAttackType::Ice)
		{
			if (ProficiencyStruct->IceProficiency >= 200.0f && ProficiencyStruct->IceLevel == 1) // Level 1 to 2
			{
				ProficiencyStruct->IceLevel = 2;
				ProficiencyStruct->IceProficiency = 0.0f;
				UnlockElementalAbilities(TheWeaponType, ElementType, 2);
			}
			else if (ProficiencyStruct->IceProficiency >= 300.0f && ProficiencyStruct->IceLevel == 2) // Level 2 to 3
			{
				ProficiencyStruct->IceLevel = 3;
				ProficiencyStruct->IceProficiency = 0.0f;
				UnlockElementalAbilities(TheWeaponType, ElementType, 3);
			}
		}
		else if (ElementType == EElementalAttackType::Thunder)
		{
			if (ProficiencyStruct->ThunderProficiency >= 200.0f && ProficiencyStruct->ThunderLevel == 1) // Level 1 to 2
			{
				ProficiencyStruct->ThunderLevel = 2;
				ProficiencyStruct->ThunderProficiency = 0.0f;
				UnlockElementalAbilities(TheWeaponType, ElementType, 2);
			}
			else if (ProficiencyStruct->ThunderProficiency >= 300.0f && ProficiencyStruct->ThunderLevel == 2) // Level 2 to 3
			{
				ProficiencyStruct->ThunderLevel = 3;
				ProficiencyStruct->ThunderProficiency = 0.0f;
				UnlockElementalAbilities(TheWeaponType, ElementType, 3);
			}
		}
	}

}





void ARen_Low_Poly_Character::UnlockElementalAbilities(EWeaponType TheWeaponType, EElementalAttackType ElementType, int32 Level)
{

	if (TheWeaponType == EWeaponType::Sword)
	{
		if (ElementType == EElementalAttackType::Fire)
		{
			if (Level == 2)
			{
				// Unlock level 2 Fire attack for Sword

				ElementalAttacks.Add(FElemental_Struct(TEXT("Fire Lv.2"), EElementalAttackType::Fire, 1.7f, 15.0f, 2, true, FireAOEAnimation));

				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Fire, unlocked Fire AOE!"))

			}
			else if (Level == 3)
			{


				ElementalAttacks.Add(FElemental_Struct(TEXT("Fire Lv.3"), EElementalAttackType::Fire, 2.7f, 25.0f, 3, true, FireGroundAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Fire, unlocked Fire Ground!"))

			}
		}
		else if (ElementType == EElementalAttackType::Ice)
		{
			if (Level == 2)
			{

				ElementalAttacks.Add(FElemental_Struct(TEXT("Ice Lv.2"), EElementalAttackType::Ice, 1.7f, 10.0f, 3, true, IceAOEAnimation));

				//ElementalAttacks.Add(FElemental_Struct(TEXT("Ice AOE"), EElementalAttackType::Ice, 1.7f, 10.0f, 2, true, IceAOEAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Ice, unlocked Ice AOE!"))


			}
			else if (Level == 3)
			{
				ElementalAttacks.Add(FElemental_Struct(TEXT("Ice Lv.3"), EElementalAttackType::Ice, 1.7f, 10.0f, 3, true, IceGroundAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Ice, unlocked Ice Ground!"))


			}
		}
		else if (ElementType == EElementalAttackType::Thunder)
		{
			if (Level == 2)
			{

				ElementalAttacks.Add(FElemental_Struct(TEXT("Thunder Lv.2"), EElementalAttackType::Thunder, 1.7f, 10.0f, 3, true, ThunderAOEAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up thunder, unlocked Thunder AOE!"))


			}
			else if (Level == 3)
			{
				ElementalAttacks.Add(FElemental_Struct(TEXT("Thunder Lv.3"), EElementalAttackType::Thunder, 1.7f, 10.0f, 3, true, ThunderGroundAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up thunder, unlocked Thunder Ground!"))

			}
		}
	}


	if (TheWeaponType == EWeaponType::Staff)
	{
		if (ElementType == EElementalAttackType::Fire)
		{

			if (Level == 2)
			{
				// Unlock level 2 Fire attack for Sword

				ElementalAttacks.Add(FElemental_Struct(TEXT("Fire Lv.2"), EElementalAttackType::Fire, 1.7f, 15.0f, 2, true, FireAOEAnimation));

				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Fire, unlocked Fire AOE!"))

			}
			else if (Level == 3)
			{


				ElementalAttacks.Add(FElemental_Struct(TEXT("Fire Lv.3"), EElementalAttackType::Fire, 2.7f, 25.0f, 3, true, FireGroundAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Fire, unlocked Fire Ground!"))

			}
		}
		else if (ElementType == EElementalAttackType::Ice)
		{
			if (Level == 2)
			{

				ElementalAttacks.Add(FElemental_Struct(TEXT("Ice Lv.2"), EElementalAttackType::Ice, 1.7f, 10.0f, 3, true, IceAOEAnimation));

				//ElementalAttacks.Add(FElemental_Struct(TEXT("Ice AOE"), EElementalAttackType::Ice, 1.7f, 10.0f, 2, true, IceAOEAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Ice, unlocked Ice AOE!"))


			}
			else if (Level == 3)
			{
				ElementalAttacks.Add(FElemental_Struct(TEXT("Ice Lv.3"), EElementalAttackType::Ice, 1.7f, 10.0f, 3, true, IceGroundAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up Ice, unlocked Ice Ground!"))


			}
		}
		else if (ElementType == EElementalAttackType::Thunder)
		{
			if (Level == 2)
			{

				ElementalAttacks.Add(FElemental_Struct(TEXT("Thunder Lv.2"), EElementalAttackType::Thunder, 1.7f, 10.0f, 3, true, ThunderAOEAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up thunder, unlocked Thunder AOE!"))


			}
			else if (Level == 3)
			{
				ElementalAttacks.Add(FElemental_Struct(TEXT("Thunder Lv.3"), EElementalAttackType::Thunder, 1.7f, 10.0f, 3, true, ThunderGroundAnimation));
				UE_LOG(LogTemp, Warning, TEXT("Leveled Up thunder, unlocked Thunder Ground!"))

			}
		}

	}
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


	ElementalAttacks.Empty(); // Clear existing attacks before adding new ones

	

	// Thunder Attack
	ElementalAttacks.Add(FElemental_Struct(
		TEXT("Fire AOE"),
		EElementalAttackType::Fire,
		1.3f,
		15.0f,
		2,
		false,
		FireAOEAnimation
	));


	ElementalAttacks.Add(FElemental_Struct(
		TEXT("Ice AOE"),
		EElementalAttackType::Ice,
		1.3f,
		15.0f,
		2,
		false,
		IceAOEAnimation
	));


	ElementalAttacks.Add(FElemental_Struct(
		TEXT("Thunder AOE"),
		EElementalAttackType::Thunder,
		1.3f,
		15.0f,
		2,
		false,
		ThunderAOEAnimation
	));

	// Populate the map with these attacks
	//ElementalAttackMap.Add(EElementalAttackType::Fire, FireAttack);
	//ElementalAttackMap.Add(EElementalAttackType::Ice, IceAttack);
	//ElementalAttackMap.Add(EElementalAttackType::Thunder, ThunderAttack);

	

}




void ARen_Low_Poly_Character::ApplyPowerUp(ESpecialPowerUp PowerUp)
{

	CurrentPowerUp = PowerUp; // Set the current power-up

	switch (PowerUp)

	{
	case ESpecialPowerUp::Berserk:
		UE_LOG(LogTemp, Warning, TEXT("Berserk Activated: Increased Attack Damage!"));

		BaseAttack *= 2.0f;
		GetWorld()->GetTimerManager().SetTimer(ResetAttackTimer, this, &ARen_Low_Poly_Character::ResetAttackPower, 15.0f, false);

		break;




	case ESpecialPowerUp::Invulnerability:
		UE_LOG(LogTemp, Warning, TEXT("Invulnerability Activated: Cannot Take Damage!"));

		GetMesh()->SetGenerateOverlapEvents(false);
		GetWorld()->GetTimerManager().SetTimer(InvulnerabilityTimer, this, &ARen_Low_Poly_Character::NullifyInvulnerability, 15.0f, false);

		break;




	case ESpecialPowerUp::TimeStop:
		UE_LOG(LogTemp, Warning, TEXT("Time Stop Activated: Slowing Down Enemies!"));
		{
			TArray<AActor*> Enemies;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy_Poly::StaticClass(), Enemies);

			for (AActor* Enemy : Enemies)

			{

				if (Enemy)

				{

					Enemy->CustomTimeDilation = 0.0f; // Slow down enemy time


				}

			}


			GetWorld()->GetTimerManager().SetTimer(TimeStopTimer, this, &ARen_Low_Poly_Character::CancelTimeStop, 15.0f, false);


			break;

		}



	case ESpecialPowerUp::DoublePoints:
		UE_LOG(LogTemp, Warning, TEXT("Double Points Activated: Score Multiplier!"));



		bDoublePoints = true;

		GetWorld()->GetTimerManager().SetTimer(InvulnerabilityTimer, this, &ARen_Low_Poly_Character::CancelDoublePoints, 20.0f, false);


		break;





	default:
		UE_LOG(LogTemp, Warning, TEXT("No Power-Up Activated."));
		break;
	}




}




void ARen_Low_Poly_Character::ResetAttackPower()
{


	BaseAttack = InitialAttack;

	UE_LOG(LogTemp, Warning, TEXT("Berserk Ended: Attack Power Reset to Initial Value (%f)."), InitialAttack);


}

void ARen_Low_Poly_Character::NullifyInvulnerability()
{

	
	GetMesh()->SetGenerateOverlapEvents(true);


}



void ARen_Low_Poly_Character::CancelTimeStop()
{

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy_Poly::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)

	{

		if (Enemy)

		{

			Enemy->CustomTimeDilation = 1.0f; // Slow down enemy time


		}

	}

}



void ARen_Low_Poly_Character::CancelDoublePoints()
{


	bDoublePoints = false;


}











void ARen_Low_Poly_Character::CheckForTechniqueUnlock(EWeaponType Weapon, int32 WeaponLevel)
{


	// Ensure the WeaponProficiencyMap is valid
	if (!WeaponProficiencyMap.Contains(WeaponType))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon type %d not found in proficiency map!"), static_cast<int32>(WeaponType));
		return;
	}

	// Check the global technique map for this weapon type
	if (WeaponLevelToTechniqueMap.Contains(WeaponType))
	{
		FWeaponTechniqueMap& TechniqueMap = WeaponLevelToTechniqueMap[WeaponType];

		// Look for a technique associated with the given level
		if (TechniqueMap.LevelToTechnique.Contains(WeaponLevel))
		{
			FString TechniqueName = TechniqueMap.LevelToTechnique[WeaponLevel];

			// Queue the technique for unlocking if not already unlocked
			if (!QueuedUnlockTechniques.Contains(TechniqueName))
			{
				QueuedUnlockTechniques.Add(TechniqueName);
				UE_LOG(LogTemp, Log, TEXT("Technique '%s' queued for unlock."), *TechniqueName);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No technique map found for weapon type %d!"), static_cast<int32>(WeaponType));
	}




}



void ARen_Low_Poly_Character::UnlockQueuedTechniques()
{


	// Check if there are any queued techniques to unlock
	if (QueuedUnlockTechniques.Num() > 0)
	{
		for (const FString& TechniqueName : QueuedUnlockTechniques)
		{
			// Find the technique by its name
			FTechnique_Struct* TechniqueToUnlock = FindTechniqueByName(TechniqueName);

			// Check if the technique exists in the array
			if (TechniqueToUnlock)
			{
				// Unlock the technique
				TechniqueToUnlock->bIsUnlocked = true;
				UE_LOG(LogTemp, Log, TEXT("Technique '%s' has been unlocked!"), *TechniqueName);
			}
			else
			{
				// Log if the technique wasn't found
				UE_LOG(LogTemp, Warning, TEXT("Technique '%s' not found in Techniques array!"), *TechniqueName);
			}
		}

		// Clear the queue after unlocking the techniques
		QueuedUnlockTechniques.Empty();
	}
	else
	{
		// Optional: Log if the queue is empty (for debugging purposes)
		UE_LOG(LogTemp, Warning, TEXT("No techniques in the queue to unlock."));
	}

}



FTechnique_Struct* ARen_Low_Poly_Character::FindTechniqueByName(const FString& TechniqueName)
{

	for (FTechnique_Struct& Technique : Techniques)
	{
		if (Technique.TechniqueName == TechniqueName)
		{
			return &Technique;
		}
	}

	return nullptr;
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

		// Add EXP to the current weapon
		Proficiency.CurrentEXP += ExpAmount;

		UE_LOG(LogTemp, Warning, TEXT("EXP after adding: %.2f"), Proficiency.CurrentEXP);


		// Check for level-up
		CheckWeaponLevelUp(WeaponType);
	}

	else
	{
		// Log a warning if WeaponType is missing from the map
		UE_LOG(LogTemp, Error, TEXT("WeaponProficiencyMap does not contain the current WeaponType!"));
	}

}




void ARen_Low_Poly_Character::CheckWeaponLevelUp(EWeaponType Weapon)
{

	if (WeaponProficiencyMap.Contains(Weapon))
	{
		FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[Weapon];

		// Check if the current EXP exceeds the threshold
		while (Proficiency.CurrentEXP >= Proficiency.EXPToNextLevel && Proficiency.WeaponLevel < 20)
		{

			Proficiency.CurrentEXP -= Proficiency.EXPToNextLevel;

			bQueuedLevelUp = true;

			break;
		}

	}
}



void ARen_Low_Poly_Character::QueueEXP(float ExpAmount)
{

	if (ExpAmount > 0)
	{
		QueuedEXP.Add(ExpAmount);
		UE_LOG(LogTemp, Log, TEXT("Queued %.2f EXP for later"), ExpAmount);
	}



}



void ARen_Low_Poly_Character::ApplyQueuedEXP()
{



	for (float ExpAmount : QueuedEXP)
	{
		AddWeaponEXP(ExpAmount); // This function already handles adding EXP and checking for level-ups
	}

	// After applying the EXP, clear the queue
	QueuedEXP.Empty();

	UE_LOG(LogTemp, Log, TEXT("Applied all queued EXP and cleared the queue."));




}



void ARen_Low_Poly_Character::ApplyQueuedLevelUp(EWeaponType Weapon)
{
	// Clear queued techniques to ensure only current level-up techniques are added
	QueuedUnlockTechniques.Empty();
	UE_LOG(LogTemp, Log, TEXT("Cleared QueuedUnlockTechniques at start of ApplyQueuedLevelUp"));

	// Check if a level-up is queued and the weapon exists in the proficiency map
	if (bQueuedLevelUp && WeaponProficiencyMap.Contains(Weapon))
	{
		// Retrieve the proficiency struct for the specified weapon
		FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[Weapon];

		// Store old stats
		PreviousAttackPower = BaseAttack;
		PreviousDefense = BaseDefence;
		PreviousElementalPower = BaseElementalAttack;
		PreviousMaxHealth = HealthStruct.MaxHealth;
		PreviousMaxMana = ManaStruct.MaxMana;

		// Increment weapon level and adjust EXP threshold
		Proficiency.WeaponLevel++;
		Proficiency.EXPToNextLevel *= 1.25f;
		bQueuedLevelUp = false;

		// Apply proficiency upgrades
		Proficiency.AttackPowerBoost += 4.f;
		Proficiency.DefenseBoost += 2.f;
		Proficiency.ElementalPowerBoost += 3.f;
		Proficiency.MaxHealthBoost += 10.f;
		Proficiency.MaxManaBoost += 15.f;

		// Update total stats dynamically (base + boost)
		UpdateStatsBasedOnWeapon();

		// Log the level-up details
		UE_LOG(LogTemp, Warning, TEXT("Weapon leveled up! Level: %d, Next EXP Threshold: %.2f"), Proficiency.WeaponLevel, Proficiency.EXPToNextLevel);

		// Unlock techniques if applicable
		if (WeaponLevelToTechniqueMap.Contains(Weapon))
		{
			FWeaponTechniqueMap& TechniqueMap = WeaponLevelToTechniqueMap[Weapon];
			for (const auto& LevelTechniquePair : TechniqueMap.LevelToTechnique)
			{
				// Ensure all techniques up to the current level are added
				if (LevelTechniquePair.Key <= Proficiency.WeaponLevel)
				{
					FString TechniqueToUnlock = LevelTechniquePair.Value;
					if (!QueuedUnlockTechniques.Contains(TechniqueToUnlock))
					{
						QueuedUnlockTechniques.Add(TechniqueToUnlock);
						UE_LOG(LogTemp, Log, TEXT("Queued technique: %s for next run."), *TechniqueToUnlock);
					}
				}
			}
		}
	}
}










float ARen_Low_Poly_Character::GetQueuedEXP() const
{
	float TotalQueuedEXP = 0.0f; 
	
	for (float ExpAmount : QueuedEXP) 
	
	{ 
		TotalQueuedEXP += ExpAmount;
	} 
	
	return TotalQueuedEXP;
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

	LoadHighScore();

	LoadPlayerProgress();

	UnlockQueuedTechniques();

	FindResultsCamera();


	InitialiseElementalAttacks();


	AbilityStruct.InitializeAbilityPoints();

	HealthStruct.InitializeHealth();
	HealthStruct.CurrentHealth = HealthStruct.MaxHealth;

	ManaStruct.InitializeMana();
	ManaStruct.CurrentMana = ManaStruct.MaxMana;

	//TechniqueStruct.CurrentGauge = 100.0f;
	TechniqueStruct.MaxGauge = 100.0f;
	TechniqueStruct.TechniquePoints = 1;
	TechniqueStruct.MaxTechniquePoints = 7;

	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		LockOnCandidates.Add(Actor);
	}


	AbilityStruct.CurrentAbilityPoints = 0.0f;


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



	if (WeaponType == EWeaponType::Sword)
	{
		// Initialize Sword techniques
		Techniques.Add(FTechnique_Struct{ TEXT("Stormstrike Flurry"), TEXT("Furious multi-strike sword combo."), true, StormStrikeFlurryAnimMontage, 1.6f, 3});

	    ElementalAttacks.Add(FElemental_Struct(TEXT("Fire"), EElementalAttackType::Fire, 1.5f, 10.0f, 1, true, FireProjectileAnimation));
		ElementalAttacks.Add(FElemental_Struct(TEXT("Ice"), EElementalAttackType::Ice, 1.6f, 20.0f, 1, true, IceProjectileAnimation));
		ElementalAttacks.Add(FElemental_Struct(TEXT("Thunder"), EElementalAttackType::Thunder, 1.8f, 20.0f, 1, true, ThunderProjectileAnimation));
		//ElementalAttacks.Add(FElemental_Struct(TEXT("Fire AOE"), EElementalAttackType::Fire, 2.4f, 30.0f, 2, false, FireAOEAnimation));
		//ElementalAttacks.Add(FElemental_Struct(TEXT("Ice AOE"), EElementalAttackType::Ice, 2.4f, 30.0f, 2, false, IceAOEAnimation));
		//ElementalAttacks.Add(FElemental_Struct(TEXT("Thunder AOE"), EElementalAttackType::Thunder, 2.4f, 30.0f, 2, false, ThunderAOEAnimation));
		



		// Check WeaponProficiencyMap and unlock techniques based on proficiency level
		if (WeaponProficiencyMap.Contains(EWeaponType::Sword))
		{
			int32 SwordWeaponLevel = WeaponProficiencyMap[EWeaponType::Sword].WeaponLevel;

			// Add sword techniques based on the level of proficiency (this should match your progression)
			if (SwordWeaponLevel >= 6)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Voltage Breaker"), TEXT("Electrifying ground-slam force field."), true, VoltageBreakerAnimMontage, 1.3f, 1 });
			}
			if (SwordWeaponLevel >= 10)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Tempest Barrage"), TEXT("Rapid flurry of strikes."), true, TempestBarrageAnimMontage, 1.7f, 1 });
			}
			if (SwordWeaponLevel >= 19)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Static Rush"), TEXT("Lightning-infused sword combo."), true, StaticRushAnimMontage, 1.9f, 1 });
			}
		}


	}





	if (WeaponType == EWeaponType::Staff)
	{
		// Initialize Staff techniques
		Techniques.Add(FTechnique_Struct{ TEXT("Meteor Strike"), TEXT("Fiery meteor devastates nearby enemies."), true, MeteorStrikeAnimMontage, 3.5f, 5});
		//Techniques.Add(FTechnique_Struct{ TEXT("Frost Rain"), TEXT("Icicles rain down, freezing foes."), true, FrostRainAnimMontage, 3.1f, 2});
	//	Techniques.Add(FTechnique_Struct{ TEXT("Feud Fang"), TEXT("Dark spikes pierce from below."), true, FeudFangAnimMontage, 3.7f, 2 });



		ElementalAttacks.Add(FElemental_Struct(TEXT("Fire"), EElementalAttackType::Fire, 1.7f, 65.0f, 1, true, FireProjectileAnimation));
		ElementalAttacks.Add(FElemental_Struct(TEXT("Ice"), EElementalAttackType::Ice, 1.9f, 15.0f, 1, true, IceProjectileAnimation));
		ElementalAttacks.Add(FElemental_Struct(TEXT("Thunder"), EElementalAttackType::Thunder, 1.5f, 10.0f, 1, true, ThunderProjectileAnimation));




		// Check WeaponProficiencyMap and unlock techniques based on proficiency level
		if (WeaponProficiencyMap.Contains(EWeaponType::Staff))
		{
			int32 StaffWeaponLevel = WeaponProficiencyMap[EWeaponType::Staff].WeaponLevel;

			// Add sword techniques based on the level of proficiency (this should match your progression)
			if (StaffWeaponLevel >= 4)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Stone Rush"), TEXT("Dark earth rises with force."), true, StoneRushAnimMontage, 2.9f, 3});
			}
			if (StaffWeaponLevel >= 16)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Frost Rain"), TEXT("Icicles rain down, freezing foes."), true, FrostRainAnimMontage, 1.7f, 3});
			}
			if (StaffWeaponLevel >= 19)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Feud Fang"), TEXT("Dark spikes pierce from below."), true, FeudFangAnimMontage, 1.9f, 3});
			}
		}
	}








	// Create the command menu widget
	if (CommandMenuWidgetClass)
	{
		CommandMenuWidget = CreateWidget<UCommand_Menu_Widget>(GetWorld(), CommandMenuWidgetClass);
		if (CommandMenuWidget)
		{
			CommandMenuWidget->AddToViewport(); // Add it to the player's viewport
			//CommandMenuWidget->SetVisibility(ESlateVisibility::Hidden); // Initially hide it
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

		// Log weapon stats
		UE_LOG(LogTemp, Log, TEXT("Sword Weapon Stats:"));
		UE_LOG(LogTemp, Log, TEXT("Level: %d"), SwordProficiency.WeaponLevel);
		UE_LOG(LogTemp, Log, TEXT("Current EXP: %.2f"), SwordProficiency.CurrentEXP);
		UE_LOG(LogTemp, Log, TEXT("EXP To Next Level: %.2f"), SwordProficiency.EXPToNextLevel);
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

		// Log weapon stats
		UE_LOG(LogTemp, Log, TEXT("Staff Weapon Stats:"));
		UE_LOG(LogTemp, Log, TEXT("Level: %d"), StaffProficiency.WeaponLevel);
		UE_LOG(LogTemp, Log, TEXT("Current EXP: %.2f"), StaffProficiency.CurrentEXP);
		UE_LOG(LogTemp, Log, TEXT("EXP To Next Level: %.2f"), StaffProficiency.EXPToNextLevel);
		UE_LOG(LogTemp, Log, TEXT("Attack Power Boost: %.2f"), StaffProficiency.AttackPowerBoost);
		UE_LOG(LogTemp, Log, TEXT("Defense Boost: %.2f"), StaffProficiency.DefenseBoost);
		UE_LOG(LogTemp, Log, TEXT("Elemental Power Boost: %.2f"), StaffProficiency.ElementalPowerBoost);
		UE_LOG(LogTemp, Log, TEXT("Max Health Boost: %.2f"), StaffProficiency.MaxHealthBoost);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sword proficiency not found!"));
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




void ARen_Low_Poly_Character::AddPoints(int32 Points)
{



	if (bDoublePoints)

	{

		Points *= 2;


	}

	PlayerScore += Points;


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
		// Set the WidgetSwitcher to display the techniques menu (index 3)
		CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(4);

		// Update visibility specifically for techniques menu
		UpdateVisibilityBasedOnIndex(4);
		bIsInventoryOpen = false;
		bIsTechniquesOpen = false;
		bIsElementalsOpen = true;
		LastFocusedButton = CommandMenuWidget->ElementalButton;

	}


	SetInputModeForUI();
	bIsInUIMode = true; // Track UI mode for techniques menu




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