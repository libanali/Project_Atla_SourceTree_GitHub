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
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"




AEnemy_AIController::AEnemy_AIController()
{

    AttackRange = 90.0f;
    EnemyNumber = 0;
    bIsAttacking = false;
    bIsClosingIn = false;
    bIsInAttackRange = false;
    LastDecisionTime = 0.0f;
    DecisionInterval = 2.0f;  // AI makes a new decision every 2 seconds
    bIsStrafing = false;

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

    StopMovement();

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

    // Get reference to the Game Mode
    ALowPoly_Survival_GameMode* GameMode = Cast<ALowPoly_Survival_GameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode == nullptr) return;

    // Retrieve all enemies currently in the game (or from SpawnedEnemies array)
    TArray<AActor*> AllEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy_Poly::StaticClass(), AllEnemies);
    int32 TotalEnemies = AllEnemies.Num();

    // Find the current enemy's index in the array
    int32 EnemyIndex = AllEnemies.Find(this->GetPawn());
    if (EnemyIndex == INDEX_NONE) return;  // If enemy not found, early exit

    // Calculate distance between the enemy and the player
    float DesiredStrafeDistance = 300.0f;  // The distance you want the enemy to keep from the player
    float CurrentDistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

    // Check if enough time has passed since the last decision
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastDecisionTime >= DecisionInterval)
    {
        // Make a new decision: whether to strafe or stay still/move closer
        float RandomChance = FMath::RandRange(0.0f, 1.0f); // Random float between 0 and 1
        bIsStrafing = RandomChance > 0.5f;  // 50% chance to strafe

        // Update the last decision time
        LastDecisionTime = CurrentTime;
    }

    // Perform strafing or standing behavior based on the current state (bIsStrafing)
    if (bIsStrafing)
    {
        // Strafing behavior
        if (CurrentDistanceToPlayer < DesiredStrafeDistance)
        {
            // Move away if too close
            FVector AwayFromPlayer = (GetPawn()->GetActorLocation() - TargetPlayer->GetActorLocation()).GetSafeNormal();
            FVector NewLocation = TargetPlayer->GetActorLocation() + (AwayFromPlayer * DesiredStrafeDistance);
            MoveToLocation(NewLocation, -1.0f, true);
        }
        else
        {
            // Calculate the strafing angle based on the enemy's index
            float StrafeAngle = 360.0f / TotalEnemies * EnemyIndex;

            // Determine the direction for strafing
            FVector StrafeDirection = FRotationMatrix(FRotator(0, StrafeAngle, 0)).GetUnitAxis(EAxis::X);

            // Calculate the new location, keeping the desired strafe distance
            FVector NewLocation = TargetPlayer->GetActorLocation() + (StrafeDirection * DesiredStrafeDistance);
            MoveToLocation(NewLocation, -1.0f, true);
        }
    }
    else
    {
        // Standing still or moving closer
        if (CurrentDistanceToPlayer > DesiredStrafeDistance + 50.0f)
        {
            // Move closer to the player to maintain pressure
            MoveToActor(TargetPlayer, DesiredStrafeDistance - 50.0f);
        }
        else
        {
            // Stay in place, giving the player a chance to attack
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

void AEnemy_AIController::DisableAI()
{

    // Disable the movement and stop the AI from updating
    AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());
    if (Enemy)
    {
        // Disable movement
        Enemy->GetCharacterMovement()->DisableMovement();

        // Optionally, disable rotation control
        Enemy->GetCharacterMovement()->bOrientRotationToMovement = false;
    }



}

void AEnemy_AIController::RestartAI()
{

    // Re-enable movement and rotation
    AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());
    if (Enemy)
    {
        // Enable movement
        Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

        // Enable rotation control again
        Enemy->GetCharacterMovement()->bOrientRotationToMovement = true;
    }

}



void AEnemy_AIController::UpdateBehaviour()
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
        UpdateBehaviour();  // Just focus on moving and attacking for now

    }
        // Ensure the enemy is always facing the player
        FacePlayer();
    
}
