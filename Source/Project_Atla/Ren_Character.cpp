// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Character.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h" 
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Curves/CurveFloat.h"
#include "Enemy.h"
#include "Animation/AnimMontage.h"
#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"






// Sets default values
ARen_Character::ARen_Character()
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
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->bUsePawnControlRotation = false;


	//Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = true;
	CameraSensitivity = 1.3f;


	//Health
	bIsDead = false;


	//Combat
	Blocking = false;
	IsCombatModeOn = false;


	//Lock-On
	bIsSoftLockEnabled = false;
	SoftLockRange = 2300.0f;
	SoftLockAngle = 340.0f;
	HeightTargetingOffest = 10.0f;
	MaxHardLockRange = 2600.0f;



	//Attack
	BaseAttack = 4.0f;
	AttackMultiplier = 0.2f;


	//Defence
	BaseDefence = 3.0f;
	DefenceMultiplier = 2.0f;


	//ATB
	CurrentATB = 0.0f;
	MaxATB = 100.0f;
	ATBFillRate = 14.5f;
	ATB_Attack_Boost = 1.2f;
	Current_ATB_Count = 0;
	Max_ATB_Count = 2;
	bCanUseAbilityOrItems = false;
	bIsPerformingAbility = false;

}



void ARen_Character::LookUp(float Value)
{

	//AddControllerPitchInput(Value * CameraSensitivity);


}

void ARen_Character::Turn(float Value)
{

	//AddControllerYawInput(Value * CameraSensitivity);

}

void ARen_Character::MoveForward(float Axis)
{

	if (!bIsDead)

	{


		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Axis);


	}


	if (Axis != 0 && !bIsDead && IsCombatModeOn)

	{

		float DeltaTime = GetWorld()->GetDeltaSeconds();
		CurrentATB = FMath::Min(CurrentATB + ATBFillRate * DeltaTime, MaxATB);

	}

}



void ARen_Character::MoveRight(float Axis)
{

	if (!bIsDead)

	{


		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Axis);

	
	}


	if (Axis != 0 && !bIsDead && IsCombatModeOn)

	{

		float DeltaTime = GetWorld()->GetDeltaSeconds();
		CurrentATB = FMath::Min(CurrentATB + ATBFillRate * DeltaTime, MaxATB);

	}

}



void ARen_Character::TakeDamage(float DamageAmount)
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

		}

	}

}


void ARen_Character::InflictDamageOnEnemy(AEnemy* Enemy)
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




void ARen_Character::IncreaseMana(float MPAmount)
{



}




void ARen_Character::StartManaIncrease()
{
}




void ARen_Character::CalculateTotalAttack()
{

	TotalAttack = BaseAttack * AttackMultiplier;

}

void ARen_Character::CalculateTotalDefence()
{

	TotalDefence = BaseDefence * DefenceMultiplier;

}






void ARen_Character::DecreaseHelath()
{

	if (HealthStruct.CurrentHealth > 0.0f)

	{

		HealthStruct.CurrentHealth -= 40.0f;

		HealthStruct.CurrentHealth = FMath::Clamp(HealthStruct.CurrentHealth, 0.0f, HealthStruct.MaxHealth);

	}
}



void ARen_Character::ATB_Increase()
{

	if (CurrentATB < MaxATB && IsCombatModeOn)
	{


		float NewATB = CurrentATB + ATB_Attack_Boost;

		CurrentATB = FMath::Clamp(NewATB, 0.0f, MaxATB);

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Boost ATB!"));


	}


}

void ARen_Character::ATB_Tracking()
{

	Current_ATB_Count = FMath::Clamp(Current_ATB_Count, 0.0, Max_ATB_Count);

	//If current atb is >= 50, increase atb_count to 1 and when current atb is Max, set it to 2.

	if (CurrentATB >= 50 && IsCombatModeOn)

	{

		Current_ATB_Count = 1;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT( "can use ability/items!"));
		bCanUseAbilityOrItems = true;


	}

	else

	{

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("cannot use ability/items!"));
		bCanUseAbilityOrItems = false;



	}


	if (CurrentATB == MaxATB)

	{

		Current_ATB_Count = 2;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Max ATB!"));


	}


}

void ARen_Character::PerformingAbility()
{


	//if bisperformingability and atb count is >= 1 then atb_count--

	if (bIsPerformingAbility && Current_ATB_Count == 1)

	{

		Current_ATB_Count--;
		//CurrentATB -= 50.0f;
		CurrentATB = 0.0f;

	}



	if (bIsPerformingAbility && Current_ATB_Count == 2)

	{

		Current_ATB_Count = 1;
		//CurrentATB -= 50.0f;
		CurrentATB = 50.0f;

	}





}




void ARen_Character::GainExperience(int32 ExpAmount)
{

	ExperiencePoints += ExpAmount;
	CheckAndTriggerLevelUp();
}

void ARen_Character::CheckAndTriggerLevelUp()
{



}

void ARen_Character::ToggleHardLock()
{

	if (bIsHardLockEnabled)

	{

		bIsHardLockEnabled = false;
		HardLockedEnemy = nullptr;

	}


	else if (bIsSoftLockEnabled && SoftLockedEnemy)

	{

		bIsHardLockEnabled = true;
		HardLockedEnemy = SoftLockedEnemy;
		ToggleHardLockOnEffects();

	}

	else

	{

		if (LockOnCandidates.Num() > 0)

		{

			HardLockedEnemy = LockOnCandidates[0];
			bIsSoftLockEnabled = false;

		}

	}
	
	
}

void ARen_Character::ToggleSoftLock()
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

		if (TargetDistance <= NearestTargetDistance && AngleToEnemy <= SoftLockAngle && !bIsHardLockEnabled)
		{

			bIsSoftLockEnabled = true;
			NearestEnemy = Actor;
			NearestTargetDistance = TargetDistance;
			GEngine->AddOnScreenDebugMessage(1, 1.3f, FColor::Green, TEXT("Soft Lock!"));
			ToggleSoftLockOnEffects();

		}
	}

	SoftLockedEnemy = NearestEnemy;
	bIsSoftLockEnabled = SoftLockedEnemy != nullptr;

	
}




void ARen_Character::OnEnemyDestroyed(AEnemy* DestroyedEnemy)
{

	if (bIsHardLockEnabled && HardLockedEnemy == DestroyedEnemy)

	{

		ToggleHardLockOnNextEnemy();

	}

}

void ARen_Character::ToggleHardLockOnNextEnemy()
{

	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), FoundEnemies);

	AEnemy* NextEnemy = nullptr;

	for (AActor* Actor : FoundEnemies)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);

		if (Enemy && Enemy != HardLockedEnemy)

		{
			NextEnemy = Enemy;
			break;
		}

	}


	HardLockedEnemy = NextEnemy;

	if(NextEnemy)

	{

		bIsHardLockEnabled = true;


	}

	else

	{

		bIsHardLockEnabled = false;

	}



}



void ARen_Character::TurnOffHardLock()
{

	HardLockedEnemy = nullptr;

	bIsHardLockEnabled = false;
}





void ARen_Character::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

// Called when the game starts or when spawned
void ARen_Character::BeginPlay()
{
	Super::BeginPlay();

	HealthStruct.InitializeHealth();
	ManaStruct.InitializeMana();
	//ATBStruct.InitialiseATB();


	CurrentMana = FMath::Clamp(CurrentMana, 0.0f, MaxMana);

	CalculateTotalAttack();



	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		LockOnCandidates.Add(Actor);
	}


	
	
	
}

// Called every frame
void ARen_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsHardLockEnabled && HardLockedEnemy)
	{
		float MaxRangeSquared = MaxHardLockRange * MaxHardLockRange;
		float DistanceSquared = FVector::DistSquared(GetActorLocation(), HardLockedEnemy->GetActorLocation());

		if (DistanceSquared > MaxRangeSquared)
		{
			bIsHardLockEnabled = false;
			HardLockedEnemy = nullptr;
		}
	}

	if (bIsHardLockEnabled && HardLockedEnemy)
	{
		// Calculate the rotation the character should look at
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HardLockedEnemy->GetActorLocation());
		TargetRotation.Pitch -= HeightTargetingOffest;

		// Interpolate between the current rotation and the target rotation using lerp
		FRotator LerpedRotation = FMath::Lerp(GetControlRotation(), TargetRotation, 5 * DeltaTime);

		// Set the control rotation to the lerped rotation
		GetController()->SetControlRotation(LerpedRotation);
	}




	ToggleSoftLock();

	ToggleHardLockOnEffects();

	ToggleSoftLockOnEffects();

	ATB_Tracking();

	PerformingAbility();


}

// Called to bind functionality to input
void ARen_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &ARen_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARen_Character::MoveRight);

	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &ARen_Character::ToggleHardLock);


}

