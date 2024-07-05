// Fill out your copyright notice in the Description page of Project Settings.


#include "Ren_Low_Poly_Character.h"
#include "Kismet/Gameplaystatics.h"

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
		//Play animation. Perhaps create an enum and allow the player to choose between abilities?


	}

}

void ARen_Low_Poly_Character::CheckAbilityUsage()
{

	
	if (AbilityStruct.CurrentAbilityPoints == AbilityStruct.MaxAbilityPoints)

	{

		bCanUseAbility = true;
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


void ARen_Low_Poly_Character::CalculateTotalDefence()
{

	TotalDefence = BaseDefence * DefenceMultiplier;

}





// Called when the game starts or when spawned
void ARen_Low_Poly_Character::BeginPlay()
{
	Super::BeginPlay();

	HealthStruct.InitializeHealth();
	AbilityStruct.InitializeAbilityPoints();

	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		LockOnCandidates.Add(Actor);
	}

	
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

