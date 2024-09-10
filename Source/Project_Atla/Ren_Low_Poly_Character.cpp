// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Low_Poly_Character.h"
#include "Kismet/Gameplaystatics.h"
#include "Enemy_Poly.h"
#include "Item_Pick_Up_Actor.h"
#include "Inventory_Component.h"
#include "Item.h"

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

	//Inventory
	InventoryComponent = CreateDefaultSubobject<UInventory_Component>(TEXT("Inventory System"));

	//Ability
	bCanUseAbility = false;
	

	//Lock-On
	bIsSoftLockEnabled = false;
	SoftLockRange = 2300.0f;
	SoftLockAngle = 340.0f;
	HeightTargetingOffest = 10.0f;

	//Health
	bIsDead = false;

	//Combat
	bInvincible = false;

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

	//InventorySystem


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
}






void ARen_Low_Poly_Character::IncreaseAbilityPoints(float Amount)
{

	AbilityStruct.CurrentAbilityPoints = FMath::Min(AbilityStruct.CurrentAbilityPoints + Amount, AbilityStruct.MaxAbilityPoints);

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



		GetMesh()->SetMaterial(0, OriginalMaterial);
		//Play animation. Perhaps create an enum and allow the player to choose between abilities?


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
		GetMesh()->SetMaterial(0, AbilityMaterial);
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("Can use ability!"));

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
		}


	}



}




void ARen_Low_Poly_Character::CalculateTotalAttack()
{

	TotalAttack = BaseAttack * AttackMultiplier;

}


void ARen_Low_Poly_Character::IncreaseAttack(float IncreaseAmount, float Duration)
{


	OriginalAttack = BaseAttack;
	BaseAttack *= IncreaseAmount;

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

}


void ARen_Low_Poly_Character::CalculateTotalDefence()
{

	TotalDefence = BaseDefence * DefenceMultiplier;

}

void ARen_Low_Poly_Character::IncreaseDefence(float IncreaseAmount, float Duration)
{

	OriginalDefence = BaseDefence;
	BaseDefence *= IncreaseAmount;

	GetWorldTimerManager().SetTimer(
		DefenceBonus,
		[this, IncreaseAmount]() { RevertAttack(IncreaseAmount); },
		Duration,
		false
	);

}

void ARen_Low_Poly_Character::RevertDefence(float IncreaseAmount)
{

	BaseDefence = OriginalDefence; 

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

void ARen_Low_Poly_Character::OnOverLapItem(AActor* OverlappedActor, AActor* OtherActor)
{

	AItem_Pick_Up_Actor* Item_PickUp = Cast<AItem_Pick_Up_Actor>(OtherActor);

	if (Item_PickUp && InventoryComponent)

	{

		UItem* Item = Item_PickUp->GetItem();
		bool bAdded = InventoryComponent->AddItem(Item);
		if (bAdded)
		{
			Item_PickUp->Destroy();
		}

	}

}






// Called when the game starts or when spawned
void ARen_Low_Poly_Character::BeginPlay()
{
	Super::BeginPlay();

	HealthStruct.InitializeHealth();
	HealthStruct.CurrentHealth = 30.0f;
	AbilityStruct.InitializeAbilityPoints();

	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		LockOnCandidates.Add(Actor);
	}


	AbilityStruct.CurrentAbilityPoints = 145.0f;

	//CharacterLevel = 1;

	OnActorBeginOverlap.AddDynamic(this, &ARen_Low_Poly_Character::OnOverLapItem);

	
}

// Called every frame
void ARen_Low_Poly_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAbilityUsage();

	ToggleSoftLock();


}

// Called to bind functionality to input
void ARen_Low_Poly_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARen_Low_Poly_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARen_Low_Poly_Character::MoveRight);


	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ARen_Low_Poly_Character::UseAbility);
}

