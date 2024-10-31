// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Low_Poly_Character.h"
#include "Kismet/Gameplaystatics.h"
#include "Enemy_Poly.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "Command_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/Widget.h"


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
	GaugeIncreaseRate = 5.7f;


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

	//Defence
	BaseDefence = 3.0f;
	DefenceMultiplier = 2.0f;

	//Elemental
	BaseElementalAttack = 3.0f;
	ElementalMultiplier = 2.0f;

	//Command
	bIsInUIMode = false;
	bIsInventoryOpen = false;
	bIsTechniquesOpen = false;
	bIsInventoryEmpty = true;
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

void ARen_Low_Poly_Character::IncreaseStats(float AdditionalHealth, float AdditionalAttack, float AdditionalDefence)
{

	float HealthAdditionalAmount = HealthStruct.MaxHealth *= AdditionalHealth;
	float AttackAdditionalAmount = BaseAttack *= AdditionalAttack;
	float DefenceAdditionalAmount = BaseDefence *= AdditionalDefence;

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

	if (bCanUseAbility)

	{

		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("Ability used!"));
		AbilityStruct.CurrentAbilityPoints = 0.0f;
		bCanUseAbility = false;
		PlayAnimMontage(AbilityAnimation, 1.0f);

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


void ARen_Low_Poly_Character::UseTechnique(int32 TechniqueIndex)
{
	if (TechniqueIndex >= 0 && TechniqueIndex < Techniques.Num())
	{
		FTechnique_Struct& SelectedTechnique = Techniques[TechniqueIndex];

		// Check if the technique is unlocked and if there are enough technique points
		if (SelectedTechnique.bIsUnlocked && TechniqueStruct.TechniquePoints >= SelectedTechnique.PointsRequired)
		{

			TechniqueStruct.TechniquePoints -= SelectedTechnique.PointsRequired; // Deduct required points

			// Additional logic for using the technique (e.g., damage bonus, animations, etc.)
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

	HealthStruct.TakeDamage(DamageAmount);



	if (HealthStruct.CurrentHealth <= 0.0f)
	{

		bIsDead = true;

		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (PlayerController)

		{

			DisableInput(PlayerController);

		}


		if (DamageAmount > 0)

		{

			CalculatedDamage = DamageAmount / (1 + TotalDefence);
			HealthStruct.CurrentHealth -= CalculatedDamage;
			HealthStruct.CurrentHealth = FMath::Clamp(HealthStruct.CurrentHealth - CalculatedDamage, 0.0f, HealthStruct.MaxHealth);
			bIsHit = true;
			DisableInput(PlayerController);

			
		}


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



void ARen_Low_Poly_Character::IncreaseAttack(float IncreaseAmount, float Duration)
{


	OriginalAttack = BaseAttack;
	BaseAttack *= IncreaseAmount;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Increase Attack"));

	GetWorldTimerManager().SetTimer(
		AttackBonus,
		[this, IncreaseAmount]() { RevertAttack(IncreaseAmount); },
		Duration,
		false
	);



}

void ARen_Low_Poly_Character::RevertAttack(float IncreaseAmount)
{

	BaseAttack = OriginalAttack;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("attack back to normal"));


}



void ARen_Low_Poly_Character::CalculateTotalDefence()
{

	TotalDefence = BaseDefence * DefenceMultiplier;

}


void ARen_Low_Poly_Character::IncreaseDefence(float IncreaseAmount, float Duration)
{

	OriginalDefence = BaseDefence;
	BaseDefence *= IncreaseAmount;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Increase Defence"));



	GetWorldTimerManager().SetTimer(
		DefenceBonus,
		[this, IncreaseAmount]() { RevertDefence(IncreaseAmount); },
		Duration,
		false
	);

}



void ARen_Low_Poly_Character::RevertDefence(float IncreaseAmount)
{

	BaseDefence = OriginalDefence; 
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Defence back to normal"));


}



void ARen_Low_Poly_Character::CalculateElementalAttack()
{

	TotalElementalAttack = BaseElementalAttack * ElementalMultiplier;

}



/*
void ARen_Low_Poly_Character::GainExperience(int32 ExpAmount)
{

	ExperiencePoints += ExpAmount;
	CheckAndTriggerLevelUp();

}

void ARen_Low_Poly_Character::CheckAndTriggerLevelUp()
{

	UE_LOG(LogTemp, Warning, TEXT("CharacterLevel before: %d"), CharacterLevel);

	if (CharacterLevel < ExperienceRequired.Num() && ExperiencePoints >= ExperienceRequired[CharacterLevel])

	{

		CharacterLevel++; //Increase Character Level
		HealthStruct.CurrentHealth = HealthStruct.MaxHealth;

		FString RowName = FString::Printf(TEXT("Row%d"), CharacterLevel);
		UE_LOG(LogTemp, Warning, TEXT("RowName: %s"), *RowName); // Print the constructed RowName for verification

		FCharacter_Attributes* Attributes = CharacterAttributesTable->FindRow<FCharacter_Attributes>(FName(*RowName), FString("1"), true);

		if (Attributes)
		{
			// Print some attributes for verification
			UE_LOG(LogTemp, Warning, TEXT("MaxHealth: %f"), Attributes->MaxHealth);


			//Assign character attributes to the attributes in the data table
			
			HealthStruct.MaxHealth = Attributes->MaxHealth;
			BaseAttack = Attributes->BaseAttack;
			BaseAttack = Attributes->BaseDefence;
			BaseElementalAttack = Attributes->BaseElemental;


			UE_LOG(LogTemp, Warning, TEXT("Character leveled up to Level %d"), CharacterLevel);
		}

		else

		{

			UE_LOG(LogTemp, Warning, TEXT("Attributes not found for Level %d"), CharacterLevel);

		}
	}


}

*/




// Called when the game starts or when spawned
void ARen_Low_Poly_Character::BeginPlay()
{
	Super::BeginPlay();

	HealthStruct.InitializeHealth();
	HealthStruct.CurrentHealth = 100.0f;
	AbilityStruct.InitializeAbilityPoints();

	TechniqueStruct.CurrentGauge = 70.0f;
	TechniqueStruct.MaxGauge = 100.0f;
	TechniqueStruct.TechniquePoints = 1;

	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		LockOnCandidates.Add(Actor);
	}


	AbilityStruct.CurrentAbilityPoints = 0.0f;

	//CharacterLevel = 1;

	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARen_Low_Poly_Character::OnOverlapWithItem);


	// Initialize techniques
	Techniques.Add(FTechnique_Struct{TEXT("Downward Slash"), TEXT("A simple attack technique."), true, DownwardSlashAnimMontage, 1.3f, 2});
	Techniques.Add(FTechnique_Struct{TEXT("Power Strike"), TEXT("A simple attack technique."), true, PowerStrikeAnimMontage, 1.3f, 2});
	Techniques.Add(FTechnique_Struct{ TEXT("Fury Strike"), TEXT("A simple attack technique."), true, FuryStrikeAnimMontage, 1.5f, 2});
	Techniques.Add(FTechnique_Struct{ TEXT("mad Strike"), TEXT("A simple attack technique."), false, FuryStrikeAnimMontage, 1.5f, 1});
	Techniques.Add(FTechnique_Struct{ TEXT("happy Strike"), TEXT("A simple attack technique."), false, FuryStrikeAnimMontage, 1.5f, 1});


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
	
	TechniqueAvailability.Init(false, Techniques.Num());



	// Bind the input action
	InputComponent->BindAction("Open Commands Menu", IE_Pressed, this, &ARen_Low_Poly_Character::ToggleCommandMenu);
	InputComponent->BindAction("Roll Dodge or Back", IE_Pressed, this, &ARen_Low_Poly_Character::HandleBackInput);

	
	
}




void ARen_Low_Poly_Character::SetItemsButtonFocus()
{


	if (CommandMenuWidget && CommandMenuWidget->ItemsButton)
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
	if (CommandMenuWidget && CommandMenuWidget->WidgetSwitcher)
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

	TechniqueStruct.CurrentGauge += GaugeIncreaseRate * DeltaTime;

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

