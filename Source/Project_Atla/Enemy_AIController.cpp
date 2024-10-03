// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_AIController.h"
#include "Enemy_Token_Manager.h"
#include "Kismet/GameplayStatics.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy_Token_Manager.h"
#include "GameFramework/Character.h"
#include "Enemy_Poly.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"




AEnemy_AIController::AEnemy_AIController()
{

    AttackRange = 90.0f;
    StrafeDistance = 500.0f;
    bIsAttacking = false;
    HasToken = false;

}



void AEnemy_AIController::BeginPlay()
{

    Super::BeginPlay();
    // Get the player character as the target
    TargetPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();

    AEnemy_Token_Manager* TheTokenManager = Cast<AEnemy_Token_Manager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemy_Token_Manager::StaticClass()));
    if (TheTokenManager)
    {
        TheTokenManager->RegisterEnemy(this);
    }
}



void AEnemy_AIController::SetTarget(AActor* NewTarget)
{


    TargetPlayer = NewTarget;


}



void AEnemy_AIController::AttackPlayer()
{
    
            APawn* ControlledPawn = GetPawn();
            if (ControlledPawn)
            {
                AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(ControlledPawn);

                if (Enemy)
                {
                    // Perform your attack logic (animations, damage, etc.)
                    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Enemy is Attacking!"));
                    bIsAttacking = true;
                    Enemy->Attack();  // Call the attack function
                }
           
        }
        else
        {
            bIsAttacking = false;  // Reset attacking state if out of range
        }
    
}



void AEnemy_AIController::StrafeAroundPlayer()
{

   if (TargetPlayer)
    {
        // Get the enemy's current location
        FVector EnemyLocation = GetPawn()->GetActorLocation();

        // Calculate the direction to the player
        FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - EnemyLocation).GetSafeNormal();

        // Calculate the right vector
        FVector RightVector = FVector::CrossProduct(DirectionToPlayer, FVector::UpVector);
        
        // Randomly choose left or right for strafing
        float RandomSign = FMath::RandBool() ? 1.0f : -1.0f;

        // Calculate the strafe location using the strafe distance
        FVector StrafeLocation = TargetPlayer->GetActorLocation() + RightVector * StrafeDistance * RandomSign;

        // Move to the strafe location
        MoveToLocation(StrafeLocation);
    }

}


void AEnemy_AIController::UpdateState()
{
    if (TargetPlayer)
    {
        float DistanceToThePlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        // Debug message to show current state
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,
            FString::Printf(TEXT("Distance: %f, HasToken: %s"), DistanceToThePlayer, HasToken ? TEXT("true") : TEXT("false")));

        // Move towards the player if too far to attack
        if (DistanceToThePlayer > AttackRange && !HasToken)
        {
            MoveToActor(TargetPlayer); // Move towards player if far away
            bIsAttacking = false;
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Chasing Player!"));
        }
        // If within attack range, attack
        else if (DistanceToThePlayer < AttackRange && HasToken)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Within attack range, attacking!"));
            AttackPlayer(); // Attack the player
            bIsAttacking = true;
            StopMovement();
        }
    }

}



void AEnemy_AIController::FacePlayer()
{

    if (!TargetPlayer) return;

    // Get the location of the enemy and the player
    FVector EnemyLocation = GetPawn()->GetActorLocation();
    FVector PlayerLocation = TargetPlayer->GetActorLocation();

    // Calculate the direction to the player
    FVector DirectionToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();

    // Get the desired rotation to face the player
    FRotator DesiredRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();

    // Set the pawn's rotation to smoothly face the player (optional: you can adjust this for smooth turning)
    GetPawn()->SetActorRotation(FMath::RInterpTo(GetPawn()->GetActorRotation(), DesiredRotation, GetWorld()->GetDeltaSeconds(), 5.0f));

}



void AEnemy_AIController::Tick(float deltaTime)
{
    
    if (TargetPlayer)
    {
        UpdateState();  // Just focus on moving and attacking for now

    }
        // Ensure the enemy is always facing the player
        FacePlayer();
    
}
