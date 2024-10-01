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


    AttackRange = 50.0f;
    StrafeDistance = 500.0f;
    bIsAttacking = false;
    HasToken = false;


}

void AEnemy_AIController::BeginPlay()
{

    Super::BeginPlay();
    // Get the player character as the target
    TargetPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();


}

void AEnemy_AIController::SetTarget(AActor* NewTarget)
{


    TargetPlayer = NewTarget;



}

void AEnemy_AIController::AttackPlayer()
{
    if (TargetPlayer && GetPawn()) // Ensure the player and pawn exist
    {
        float DistanceToTargetPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        // Check if within attack range
        if (DistanceToTargetPlayer <= AttackRange)
        {
            // Perform your attack logic (animations, damage, etc.)
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Enemy is Attacking!"));
            bIsAttacking = true;  // Mark as attacking
        }
        else
        {
            // Move towards the player to get within attack range
            MoveToActor(TargetPlayer, AttackRange);
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Moving to attack range!"));
            bIsAttacking = false;  // Not attacking while moving
        }
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
        DistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        // Check if within attacking range
        if (DistanceToPlayer <= AttackRange && HasToken)
        {
            UE_LOG(LogTemp, Warning, TEXT("Within Attack Range. Preparing to attack."));
            AttackPlayer(); // Attack the player if the enemy has a token
        }
        else if (DistanceToPlayer <= StrafeDistance && !HasToken)
        {
            UE_LOG(LogTemp, Warning, TEXT("Within Strafing Distance. Strafing around player."));
            StrafeAroundPlayer(); // Strafe around the player if not in attack range and without a token
        }
        else if (DistanceToPlayer > StrafeDistance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Chasing player."));
            MoveToLocation(TargetPlayer->GetActorLocation()); // Move towards player if far away
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
    // Ensure there's a valid player target
    if (TargetPlayer)
    {
        // If already attacking, don't re-trigger
        if (!bIsAttacking)
        {
            UpdateState();
        }

        else

        {
            MoveToActor(TargetPlayer, AttackRange);


        }
    }

    FacePlayer();

}
