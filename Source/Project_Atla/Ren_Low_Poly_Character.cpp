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


	//Ability
	bCanUseAbility = false;
	bIncreaseAbilityPoints = true;
	ActivatePlungeRadius = false;


	//Technique
	GaugeIncreaseRate = 35.7f;
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
	AttackMultiplier = 2.0f; 
	AttackIncrease = 3.0f;


	//Defence
	BaseDefence = 3.0f;
	DefenceMultiplier = 2.0f;
	DefenceIncrease = 2.0f;


	//Elemental
	BaseElementalAttack = 4.0f;
	ElementalMultiplier = 2.0f;


	//Command
	bIsInUIMode = false;
	bIsInventoryOpen = false;
	bIsTechniquesOpen = false;
	bIsInventoryEmpty = true;


	//Weapon Proficiency
	QueuedUnlockTechniques = TArray<FString>();

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

		CalculatedDamage = AttackDamage * (1 - Enemy->DefencePercentage);

		UWorld* World = GetWorld();

		if (World)

		{

			float ActualDamageApplied = Enemy->ApplyDamage(CalculatedDamage, FHitResult(), GetController(), this);

		}


	}


}

void ARen_Low_Poly_Character::InflictElementalDamageOnEnemy(AEnemy_Poly* Enemy)
{


	if (Enemy)

	{

		float ElementalDamage = BaseElementalAttack;

		CalculatedDamage = ElementalDamage * (1 - Enemy->DefencePercentage);

		UWorld* World = GetWorld();

		if (World)

		{

			float ActualDamageApplied = Enemy->ApplyDamage(CalculatedDamage, FHitResult(), GetController(), this);

		}


	}



}

void ARen_Low_Poly_Character::UpdateStatsBasedOnWeapon()
{

	if (WeaponType == EWeaponType::Sword)

	{

		BaseAttack = 5.0f;
		BaseDefence = 2.0f;
		BaseElementalAttack = 4.0f;
		HealthStruct.MaxHealth = 140.0f;

	}

	else if (WeaponType == EWeaponType::Staff)

	{

		BaseAttack = 3.0f;
		BaseDefence = 2.0f;
		BaseElementalAttack = 4.0f;
		HealthStruct.MaxHealth = 130.0f;

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
			PlayAnimMontage(VictoryAnim, 1.0f); // Cheer animation
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

	// Display Game Over UI only once
	DisplayGameOverUI();


}





void ARen_Low_Poly_Character::RecoverHealth()
{

	HealthStruct.CurrentHealth = HealthStruct.MaxHealth;

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
			float TechniqueDamageBonus = Techniques[TechniqueIndex].DamageBonus;

			// Apply the technique's damage bonus
			float FinalDamage = BaseDamage * TechniqueDamageBonus;

			// Adjust for the enemy's defense
			FinalDamage *= (1 - Enemy->DefencePercentage);

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
		TechniqueStruct.CurrentGauge = 0.0f;

	}


}



void ARen_Low_Poly_Character::CheckTechniquePointsMaximum()
{

	if (TechniqueStruct.TechniquePoints == TechniqueStruct.MaxTechniquePoints)

	{

		bIsTechniquePointsMax = true;
		//TechniqueStruct.CurrentGauge = 99.0f;


	}

	else

	{

		bIsTechniquePointsMax = false;


	}

}



void ARen_Low_Poly_Character::StopFillingGauge()
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

			TechniqueStruct.TechniquePoints -= SelectedTechnique.PointsRequired; // Deduct required points

			PlayAnimMontage(SelectedTechnique.TechniqueAnimation);

			


			// Log success
			UE_LOG(LogTemp, Log, TEXT("Technique %s used, %d points deducted."), *SelectedTechnique.TechniqueName, SelectedTechnique.PointsRequired);
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

	if (bQueuedLevelUp && WeaponProficiencyMap.Contains(Weapon))

	{

		FWeapon_Proficiency_Struct& Proficiency = WeaponProficiencyMap[Weapon];

		Proficiency.WeaponLevel++;
		Proficiency.EXPToNextLevel *= 2.25f;
		bQueuedLevelUp = false;


		if (WeaponLevelToTechniqueMap.Contains(Weapon))

		{

			FWeaponTechniqueMap& TechniqueMap = WeaponLevelToTechniqueMap[Weapon];

			if (TechniqueMap.LevelToTechnique.Contains(Proficiency.WeaponLevel))

			{

				FString TechniqueToUnlock = TechniqueMap.LevelToTechnique[Proficiency.WeaponLevel];

				QueuedUnlockTechniques.Add(TechniqueToUnlock);

				UE_LOG(LogTemp, Log, TEXT("Queued technique: %s for next run."), *TechniqueToUnlock);
			}


			Proficiency.AttackPowerBoost += 4.f;
			Proficiency.DefenseBoost += 2.f;
			Proficiency.ElementalPowerBoost += 3.f;
			Proficiency.MaxHealthBoost += 10.f;


			UE_LOG(LogTemp, Warning, TEXT("Weapon leveled up! Level: %d, Next EXP Threshold: %.2f"), Proficiency.WeaponLevel, Proficiency.EXPToNextLevel);

			for (const FString& TechniqueName : QueuedUnlockTechniques)
			{

				UnlockTechnique(TechniqueName);
				UE_LOG(LogTemp, Log, TEXT("Unlocked technique: %s"), *TechniqueName);

				APlayerController* PC = Cast<APlayerController>(GetController());

				if (PC)

				{
					if (GameOverWidgetInstance)

					{

						FString NotificationMessage = FString::Printf(TEXT("Unlocked Technique: %s"), *TechniqueName);
						GameOverWidgetInstance->ShowNotification(NotificationMessage);

					}


				}
			}

			QueuedUnlockTechniques.Empty();
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
	for (const TPair<AEnemy_Poly*, UEnemy_Detection_Arrow*>& EnemyArrowPair : EnemyArrowMap)
	{
		AEnemy_Poly* Enemy = EnemyArrowPair.Key;
		UEnemy_Detection_Arrow* ArrowWidget = EnemyArrowPair.Value;

		if (Enemy && ArrowWidget)
		{
			// Call the function to check the position and update the arrow widget
			CheckAndDisplayArrow(Enemy, ArrowWidget);
		}
	}

}



void ARen_Low_Poly_Character::CheckAndDisplayArrow(AActor* Enemy, UEnemy_Detection_Arrow* ArrowWidget)
{

	// Safely cast the enemy to AEnemy_Poly to access its properties
	AEnemy_Poly* EnemyPoly = Cast<AEnemy_Poly>(Enemy);
	if (!EnemyPoly)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy is not of type AEnemy_Poly!"));
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

	HealthStruct.InitializeHealth();
	HealthStruct.CurrentHealth = HealthStruct.MaxHealth;
	AbilityStruct.InitializeAbilityPoints();

	//TechniqueStruct.CurrentGauge = 100.0f;
	TechniqueStruct.MaxGauge = 100.0f;
	TechniqueStruct.TechniquePoints = 5;
	TechniqueStruct.MaxTechniquePoints = 7;

	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		LockOnCandidates.Add(Actor);
	}


	AbilityStruct.CurrentAbilityPoints = 0.0f;

	UpdateStatsBasedOnWeapon();

	// Ensure WeaponProficiencyMap has entries for all weapon types, even if not loaded
	if (!WeaponProficiencyMap.Contains(EWeaponType::Sword))
	{
		WeaponProficiencyMap.Add(EWeaponType::Sword, FWeapon_Proficiency_Struct());
	}
	if (!WeaponProficiencyMap.Contains(EWeaponType::Staff))
	{
		WeaponProficiencyMap.Add(EWeaponType::Staff, FWeapon_Proficiency_Struct());
	}



	// Create and populate Sword techniques
	FWeaponTechniqueMap SwordTechniquesForMap;
	SwordTechniquesForMap.LevelToTechnique.Add(2, TEXT("Voltage Breaker"));
	SwordTechniquesForMap.LevelToTechnique.Add(3, TEXT("Tempest Barrage"));
	SwordTechniquesForMap.LevelToTechnique.Add(4, TEXT("Static Rush"));

	// Add Sword techniques to the main map
	WeaponLevelToTechniqueMap.Add(EWeaponType::Sword, SwordTechniquesForMap);

	// Create and populate Staff techniques
	FWeaponTechniqueMap StaffTechniquesForMap;
	StaffTechniquesForMap.LevelToTechnique.Add(2, TEXT("Inferno Rain"));


	// Add Staff techniques to the main map
	WeaponLevelToTechniqueMap.Add(EWeaponType::Staff, StaffTechniquesForMap);


	if (WeaponType == EWeaponType::Sword)
	{
		// Initialize Sword techniques
		Techniques.Add(FTechnique_Struct{ TEXT("Stormstrike Flurry"), TEXT("A simple attack technique."), true, StormStrikeFlurryAnimMontage, 1.6f, 1 });

		// Check WeaponProficiencyMap and unlock techniques based on proficiency level
		if (WeaponProficiencyMap.Contains(EWeaponType::Sword))
		{
			int32 TheWeaponLevel = WeaponProficiencyMap[EWeaponType::Sword].WeaponLevel;

			// Add sword techniques based on the level of proficiency (this should match your progression)
			if (TheWeaponLevel >= 2)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Voltage Breaker"), TEXT("A simple attack technique."), true, VoltageBreakerAnimMontage, 1.3f, 1 });
			}
			if (TheWeaponLevel >= 3)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Tempest Barrage"), TEXT("A simple attack technique."), true, TempestBarrageAnimMontage, 1.7f, 1 });
			}
			if (TheWeaponLevel >= 4)
			{
				Techniques.Add(FTechnique_Struct{ TEXT("Static Rush"), TEXT("A simple attack technique."), true, StaticRushAnimMontage, 1.9f, 1 });
			}
		}
	}

	if (WeaponType == EWeaponType::Staff)
	{
		// Initialize Staff techniques
		Techniques.Add(FTechnique_Struct{ TEXT("Inferno Rain"), TEXT("A simple attack technique."), true, InfernoRainAnimMontage, 1.5f, 2 });

		// Check WeaponProficiencyMap and unlock techniques based on proficiency level
		if (WeaponProficiencyMap.Contains(EWeaponType::Staff))
		{
			int32 TheWeaponLevel = WeaponProficiencyMap[EWeaponType::Staff].WeaponLevel;

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
			UpdateVisibilityBasedOnIndex(1);  // Update visibility right after switching to index 1

			// Make sure buttons are visible
			CommandMenuWidget->ItemsButton->SetVisibility(ESlateVisibility::Visible);
			CommandMenuWidget->TechniquesButton->SetVisibility(ESlateVisibility::Visible);

			// Add slight delay before setting keyboard focus to ensure UI updates
			GetWorldTimerManager().SetTimerForNextTick(this, &ARen_Low_Poly_Character::SetItemsButtonFocus);

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

			
			GetWorldTimerManager().SetTimerForNextTick(this, &ARen_Low_Poly_Character::SetItemsButtonFocus);
			

			bIsInventoryOpen = false;
			bIsTechniquesOpen = false;
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
		bIsTechniquesOpen = true;


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
			// Return to command icon
			CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(0);
			SetInputModeForGameplay();
			bIsInUIMode = false; // Return to gameplay
		}

		else if (CurrentIndex == 2) // If in inventory

		{
			// Go back to the main command menu
			CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(1);
			bIsInUIMode = true; // Still in UI mode

			GetWorldTimerManager().SetTimerForNextTick(this, &ARen_Low_Poly_Character::SetItemsButtonFocus);
			

		}

		else if (CurrentIndex == 3)

		{
			CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(1);
			bIsInUIMode = true;
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

	StopFillingGauge();

	UpdateEnemyArrows();

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