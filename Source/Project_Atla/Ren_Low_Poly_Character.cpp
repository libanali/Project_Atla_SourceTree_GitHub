// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Low_Poly_Character.h"
#include "Kismet/Gameplaystatics.h"
#include "Enemy_Poly.h"
#include "Components/CapsuleComponent.h"

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

	//Level
	CharacterLevel = 1;
	ExperienceRequired.Add(0); //Level 1
	ExperienceRequired.Add(60); //Level 2
	ExperienceRequired.Add(100);//Level 3
	ExperienceRequired.Add(180);//Level 4
	ExperienceRequired.Add(240);//Level 5


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



void ARen_Low_Poly_Character::InflictAbilityDamageOnEnemy(AEnemy_Poly* Enemy, int32 TechniqueIndex)
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
			// Use the technique logic here
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







void ARen_Low_Poly_Character::UnlockTechnique4()
{

	Techniques.Add(FTechnique_Struct{ TEXT("crazy Strike"), TEXT("A simple attack technique."), true, FuryStrikeAnimMontage, 1.5f, 1 });



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






// Called when the game starts or when spawned
void ARen_Low_Poly_Character::BeginPlay()
{
	Super::BeginPlay();

	HealthStruct.InitializeHealth();
	HealthStruct.CurrentHealth = 100.0f;
	AbilityStruct.InitializeAbilityPoints();

	TechniqueStruct.CurrentGauge = 0.0f;
	TechniqueStruct.MaxGauge = 100.0f;
	TechniqueStruct.TechniquePoints = 3;

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
	Techniques.Add(FTechnique_Struct{TEXT("Downward Slash"), TEXT("A simple attack technique."), true, DownwardSlashAnimMontage, 1.3f, 1});
	Techniques.Add(FTechnique_Struct{TEXT("Power Strike"), TEXT("A simple attack technique."), true, PowerStrikeAnimMontage, 1.3f, 2});
	Techniques.Add(FTechnique_Struct{ TEXT("Fury Strike"), TEXT("A simple attack technique."), false, FuryStrikeAnimMontage, 1.5f, 1});
	Techniques.Add(FTechnique_Struct{ TEXT("mad Strike"), TEXT("A simple attack technique."), false, FuryStrikeAnimMontage, 1.5f, 1 });
	//Techniques.Add(FTechnique_Struct{ TEXT("crazy Strike"), TEXT("A simple attack technique."), false, FuryStrikeAnimMontage, 1.5f, 1 });
	Techniques.Add(FTechnique_Struct{ TEXT("happy Strike"), TEXT("A simple attack technique."), false, FuryStrikeAnimMontage, 1.5f, 1 });




	
	
}

// Called every frame
void ARen_Low_Poly_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAbilityUsage();

	CheckGaugeMaximum();

	ToggleSoftLock();

	TechniqueStruct.CurrentGauge += GaugeIncreaseRate * DeltaTime;



}

// Called to bind functionality to input
void ARen_Low_Poly_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARen_Low_Poly_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARen_Low_Poly_Character::MoveRight);


	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ARen_Low_Poly_Character::UseAbility);

}

