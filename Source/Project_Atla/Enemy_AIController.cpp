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
    bIsClosingIn = false;
    bIsInAttackRange = false;

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
{  
   // If attack is on cooldown, do nothing
    if (bIsAttackOnCooldown)
    {
        return;
    }

    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn)
    {
        AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(ControlledPawn);
        if (Enemy && TargetPlayer)
        {
            // Calculate distance to the player
            float DistanceToThePlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

            // Check if enemy is within attack range
            if (DistanceToThePlayer <= AttackRange)
            {
                // Log the attack and play the attack animation
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Enemy is Attacking!"));
                bIsAttacking = true;
                bIsAttackOnCooldown = true;  // Set the cooldown flag
                Enemy->Attack();  // Call the attack function to play the animation
                StopMovement();

                // Set a timer to reset attack cooldown after a delay (e.g., 2 seconds)
                GetWorld()->GetTimerManager().SetTimer(AttackDelayHandle, this, &AEnemy_AIController::ResetAttackCooldown, 2.0f, false);
            }
            else
            {
                // Enemy is not in range, move closer to the player
                MoveToActor(TargetPlayer, AttackRange - 50.0f);  // Move closer before attacking
            }
        }
    }
}


void AEnemy_AIController::StrafeAroundPlayer()
{

    if (TargetPlayer == nullptr || !GetPawn()) return;

    // Determine the desired strafe distance from the player
    float DesiredStrafeDistance = 280.0f;  // The distance you want the enemy to keep from the player
    float CurrentDistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

    // If the enemy is too close, move it further away
    if (CurrentDistanceToPlayer < DesiredStrafeDistance)
    {
        // Calculate the direction away from the player
        FVector AwayFromPlayer = (GetPawn()->GetActorLocation() - TargetPlayer->GetActorLocation()).GetSafeNormal();

        // Calculate a new location further away
        FVector NewLocation = TargetPlayer->GetActorLocation() + (AwayFromPlayer * DesiredStrafeDistance);

        // Move the enemy to the new location
        MoveToLocation(NewLocation, -1.0f, true);
    }
    else
    {
        // Strafe around the player while maintaining distance
        FVector Direction = (TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();

        // Get a perpendicular vector for strafing (rotate 90 degrees)
        FVector StrafeDirection = FVector(-Direction.Y, Direction.X, 0.0f); // Right strafe

        // Calculate the new strafe location, keeping the desired distance
        FVector NewLocation = TargetPlayer->GetActorLocation() + (StrafeDirection * DesiredStrafeDistance);

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

    ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());

    if (TargetPlayer == nullptr || !GetPawn()) return;

    // Attack only if this enemy is the current attacker
    if (GameMode->CurrentAttacker == this)
    {
        // Calculate distance to the player
        float DistanceToThePlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        // If not in range, move closer to the player first
        if (DistanceToThePlayer > AttackRange)
        {
            MoveToActor(TargetPlayer, AttackRange - 80.0f);  // Adjust to stop slightly within attack range
        }
        else
        {
            // Stop moving and initiate the attack
            StopMovement();
            AttackPlayer();
        }
    }
    else
    {
        // Strafe if not attacking
        StrafeAroundPlayer();
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
