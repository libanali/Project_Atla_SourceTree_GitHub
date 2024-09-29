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
            Enemy->Attack();  // Make sure Attack() is implemented in AEnemy_Poly
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Enemy is Attacking!"));

            bIsAttacking = true;
        }
    }


}

void AEnemy_AIController::StrafeAroundPlayer()
{

    if (TargetPlayer)
    {
        // Get the enemy's current rotation
        FRotator EnemyRotation = GetPawn()->GetActorRotation();

        // Calculate the right vector from the rotation
        FVector RightVector = FRotationMatrix(EnemyRotation).GetScaledAxis(EAxis::Y);

        // Calculate the new strafe location
        FVector StrafeLocation = TargetPlayer->GetActorLocation() + RightVector * 200; // Circle radius

        // Move to the strafe location
        MoveToLocation(StrafeLocation);
    }

}

void AEnemy_AIController::UpdateState()
{

    if (TargetPlayer)
    {
        DistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        // Debug the distance
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Distance to Player: %f"), DistanceToPlayer));

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
