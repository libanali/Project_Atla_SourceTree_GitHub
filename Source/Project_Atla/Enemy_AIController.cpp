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
        float DistanceToThePlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        if (DistanceToThePlayer <= AttackRange)
        {
            AttackPlayer();
        }
    }
    else
    {
        StrafeAroundPlayer();  // Strafe if not attacking
    }
}

void AEnemy_AIController::SetEnemyNumber(int32 NewNumber)
{


    EnemyNumber = NewNumber;

}

void AEnemy_AIController::UpdateEnemyNumbers()
{

    

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
