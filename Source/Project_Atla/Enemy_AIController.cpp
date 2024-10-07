// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_AIController.h"
#include "Enemy_Token_Manager.h"
#include "Kismet/GameplayStatics.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy_Token_Manager.h"
#include "GameFramework/Character.h"
#include "Enemy_Poly.h"
#include "LowPoly_Survival_GameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"




AEnemy_AIController::AEnemy_AIController()
{

    AttackRange = 90.0f;
    EnemyNumber = 0;
    bIsAttacking = false;

}



void AEnemy_AIController::BeginPlay()
{

    Super::BeginPlay();
    // Get the player character as the target
    TargetPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();
    EnemyNumber = 0;

}



void AEnemy_AIController::SetTarget(AActor* NewTarget)
{


    TargetPlayer = NewTarget;


}



void AEnemy_AIController::AttackPlayer()
{  // If attack is on cooldown, do nothing
    if (bIsAttackOnCooldown)
    {
        return;
    }

    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn)
    {
        AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(ControlledPawn);
        if (Enemy)
        {
            // Log the attack and play the attack animation
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Enemy is Attacking!"));
            bIsAttacking = true;
            bIsAttackOnCooldown = true;  // Set the cooldown flag
            Enemy->Attack();  // Call the attack function to play the animation

            // Set a timer to reset attack cooldown after a delay (e.g., 2 seconds)
            GetWorld()->GetTimerManager().SetTimer(AttackDelayHandle, this, &AEnemy_AIController::ResetAttackCooldown, 2.0f, false);
        }
    }
}


void AEnemy_AIController::StrafeAroundPlayer()
{

   if (TargetPlayer)
    {
       // Determine the desired strafe distance
       float StrafeDistance = 300.0f; // Adjust as necessary for your gameplay
       FVector Direction = (TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();

       // Get a perpendicular vector for strafing (rotate 90 degrees)
       FVector StrafeDirection = FVector(-Direction.Y, Direction.X, 0.0f); // Right strafe

       // Calculate the new strafe location
       FVector NewLocation = TargetPlayer->GetActorLocation() + StrafeDirection * StrafeDistance;

       // Move to the new location
       MoveToLocation(NewLocation, -1.0f, true);
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

void AEnemy_AIController::ResetAttackCooldown()
{

    bIsAttacking = false;
    bIsAttackOnCooldown = false;


    // Get the game mode and update enemy numbers
    if (GetWorld())
    {
        ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->CycleToNextEnemy();
        }
    }


    // After cooldown, start strafing or moving again
    if (!bIsAttacking)  // Ensure it's not in attack mode
    {
        StrafeAroundPlayer();  // Resume movement, like strafing
    }

}



void AEnemy_AIController::UpdateBehavior()
{

    // Cast the game mode to your specific game mode class
    ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());

    if (TargetPlayer == nullptr || !GetPawn() || GameMode == nullptr) return;

    // Get distance to the player
    float DistanceToThePlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

    // Check if this enemy is the current attacker
    if (GameMode->CurrentAttacker == this)
    {
        // Check if the enemy is within attack range
        if (DistanceToThePlayer <= AttackRange)
        {
            AttackPlayer();  // Attack if within range
        }
        else
        {
            // Move closer to the player if out of range
            MoveToActor(TargetPlayer, -1.0f, true);
        }
    }
    else
    {
        // Non-attacking enemies should strafe around the player
        StrafeAroundPlayer();  // Call your strafing behavior
    }
}



void AEnemy_AIController::SetEnemyNumber(int32 NewNumber)
{


    EnemyNumber = NewNumber;

}



int32 AEnemy_AIController::GetEnemyNumber() const
{
    return EnemyNumber;
}



void AEnemy_AIController::Tick(float deltaTime)
{
    
    if (TargetPlayer)
    {
        UpdateBehavior();  // Just focus on moving and attacking for now

    }
        // Ensure the enemy is always facing the player
        FacePlayer();
    
}
