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
    StrafeDistance = 900.0f;
    bIsAttacking = false;


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

    if (TargetPlayer)
    {
        AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());

        if (Enemy)
        {
            float DistanceToTargetPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

            // Check if within attack range
            if (DistanceToTargetPlayer <= AttackRange)
            {
                Enemy->Attack();  // Make sure Attack() is implemented in AEnemy_Poly
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

}

void AEnemy_AIController::StrafeAroundPlayer()
{

    if (TargetPlayer)
    {
        FVector PlayerLocation = TargetPlayer->GetActorLocation();

        // Calculate the desired strafe distance
        float DesiredDistance = StrafeDistance;

        // Get a random angle for strafing
        float RandomAngle = FMath::RandRange(0.f, 360.f);
        FVector StrafeDirection = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
            FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
            0.f);

        // Calculate the strafe location based on the player's position and desired distance
        FVector StrafeLocation = PlayerLocation + StrafeDirection * DesiredDistance;

        // Move the enemy to the calculated strafe location
        MoveToLocation(StrafeLocation);
    }

}

void AEnemy_AIController::UpdateState()
{

    if (TargetPlayer)
    {
        DistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());


        if (DistanceToPlayer <= AttackRange)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Within Attack Range"));
            AttackPlayer();
        }
        else
        {
            StrafeAroundPlayer();
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
