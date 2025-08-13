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
    bIsFrozen = false;
    bIsStunned = false;
    bIsAIStopped = false;

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
               // GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Enemy is Attacking!"));
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
    if (!TargetPlayer || !GetPawn()) return;

    // Only rotate if the flag allows it
    AEnemy_Poly* EnemyPawn = Cast<AEnemy_Poly>(GetPawn());
    if (EnemyPawn && EnemyPawn->bShouldFacePlayer)
    {
        // Get the location of the enemy and the player
        FVector EnemyLocation = EnemyPawn->GetActorLocation();
        FVector PlayerLocation = TargetPlayer->GetActorLocation();

        // Calculate the direction to the player
        FVector DirectionToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();

        // Get the desired rotation to face the player
        FRotator DesiredRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();

        // Set the pawn's rotation to smoothly face the player (optional: you can adjust this for smooth turning)
        EnemyPawn->SetActorRotation(FMath::RInterpTo(EnemyPawn->GetActorRotation(), DesiredRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
    }
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




void AEnemy_AIController::UpdateHitState()
{

    AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());
    if (!Enemy) return;

    // Check if the hit state has changed
    if (Enemy->bEnemyIsHit)
    {
        
            // Disable AI if the enemy is hit
            DisableAI();
           // GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Enemy is not moving after hit!"));
    }

     else

    {
            // Restart AI if the enemy is no longer hit
            RestartAI();
           // GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Enemy is moving after hit!"));
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
        // Add this line to stop any in-progress movement
        StopMovement();
        bIsAIStopped = true;
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
        // Reset the stopped flag
        bIsAIStopped = false;
        // Resume any behavior like strafing
        if (TargetPlayer)
        {
            // Start the AI moving again based on current situation
           // UpdateBehaviour();
        }
    }

}

void AEnemy_AIController::ApplyDirectApproach(float DeltaTime)
{

    // Simple direct movement toward player for all enemy types when far away
    MovementPatternTimer -= DeltaTime;

    if (MovementPatternTimer <= 0.0f)
    {
        MovementPatternTimer = FMath::RandRange(1.0f, 2.0f);

        // Move directly toward player
        MoveToActor(TargetPlayer, 100.0f);
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

EEnemyType AEnemy_AIController::GetEnemyType() const
{
    return EEnemyType();
}

void AEnemy_AIController::SetEnemyType(EEnemyType NewType)
{
    EnemyType = NewType;

}

void AEnemy_AIController::InitializeForEnemyType(EEnemyType Type)
{
    EnemyType = Type;

    // Reset the attack probability
    AttackProbability = 0.0f;

    // Set movement speed based on type
    AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());
    if (Enemy && Enemy->GetCharacterMovement())
    {
        switch (Type)
        {
        case EEnemyType::Spider:
            Enemy->GetCharacterMovement()->MaxWalkSpeed = 500.0f;
            break;

        case EEnemyType::Wolf:
            Enemy->GetCharacterMovement()->MaxWalkSpeed = 450.0f;
            break;

        case EEnemyType::RockTroll:
            Enemy->GetCharacterMovement()->MaxWalkSpeed = 300.0f;
            break;
        }
    }

    // Initialize movement pattern timer
    MovementPatternTimer = 0.0f;
}

void AEnemy_AIController::SetTotalEnemyCount(int32 Count)
{
    TotalEnemyCount = FMath::Max(1, Count);

}

void AEnemy_AIController::SetBaseAttackProbability(float Probability)
{
    BaseAttackProbability = Probability;

}

void AEnemy_AIController::SetAggressionFactor(float Factor)
{
    AggressionFactor = Factor;

}

void AEnemy_AIController::UpdateBehaviour(float DeltaTime)
{
    if (TargetPlayer == nullptr || !GetPawn()) return;

    AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());
    if (!Enemy) return;

    // Calculate distance to player
    float TheDistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

    // Update attack probability (increases over time)
    float AttackRate = GetAttackRateForEnemyType(EnemyType);
    AttackProbability += DeltaTime * AttackRate * BaseAttackProbability * AggressionFactor;

    // Modify probability based on distance (closer = higher chance)
    float DistanceFactor = FMath::Clamp(1.0f - (TheDistanceToPlayer / 500.0f), 0.1f, 1.0f);
    float FinalAttackProbability = AttackProbability * DistanceFactor;

    // If close enough and probability check passes, attack
    if (TheDistanceToPlayer <= AttackRange && !bIsAttackOnCooldown)
    {
        float RandValue = FMath::FRand();
        if (RandValue < FinalAttackProbability * DeltaTime)
        {
            // Reset probability and attack
            AttackProbability = 0.0f;
            StopMovement();
            AttackPlayer();
            return;
        }
    }

    // Define range thresholds
    float ZigZagRange = 1000.0f;  // Distance at which spiders start zigzagging
    float MediumRange = AttackRange * 1.5f;

    // Handle movement based on distance and enemy type
    if (TheDistanceToPlayer > ZigZagRange)
    {
        // FAR DISTANCE - All enemies move directly toward player
        ApplyDirectApproach(DeltaTime);
    }
    else if (TheDistanceToPlayer > MediumRange)
    {
        // MEDIUM-FAR DISTANCE - Type-specific tactical movement
        switch (EnemyType)
        {
        case EEnemyType::Spider:
            ApplyErraticMovement(DeltaTime);  // Now only zigzags when closer
            break;

        case EEnemyType::Wolf:
            ApplyCirclingMovement(DeltaTime);
            break;

        case EEnemyType::RockTroll:
            ApplyDirectMovement(DeltaTime);  // Trolls keep moving directly
            break;
        }
    }
    else if (TheDistanceToPlayer > AttackRange)
    {
        // CLOSE DISTANCE - Prepare for attack
        PrepareForAttack(EnemyType, DeltaTime);
    }
    else
    {
        // ATTACK RANGE - Evaluate attack or reposition
        float RepositionChance = 0.3f * DeltaTime;
        if (FMath::FRand() < RepositionChance)
        {
            PrepareForAttack(EnemyType, DeltaTime);
        }
    }
}



void AEnemy_AIController::ApplyErraticMovement(float DeltaTime)
{// Spider movement - erratic zigzag pattern
    MovementPatternTimer -= DeltaTime;

    if (MovementPatternTimer <= 0.0f)
    {
        // Time to change direction
        MovementPatternTimer = FMath::RandRange(0.5f, 1.5f);

        // Calculate base direction toward player
        FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();

        // Add randomness to direction (zigzag)
        float RandomAngle = FMath::RandRange(-60.0f, 60.0f);
        FVector RandomDirection = DirectionToPlayer.RotateAngleAxis(RandomAngle, FVector(0, 0, 1));

        // Calculate target point
        float AdvanceDistance = FMath::RandRange(100.0f, 300.0f);
        CurrentMoveTarget = GetPawn()->GetActorLocation() + (RandomDirection * AdvanceDistance);

        // Move to the calculated point
        MoveToLocation(CurrentMoveTarget, -1.0f, true, true);
    }
}

void AEnemy_AIController::ApplyCirclingMovement(float DeltaTime)
{    // Wolf movement - circle around player while approaching
    MovementPatternTimer -= DeltaTime;

    if (MovementPatternTimer <= 0.0f)
    {
        // Time to update circling position
        MovementPatternTimer = FMath::RandRange(1.0f, 2.0f);

        // Calculate position in a circle around player
        float CircleRadius = FMath::RandRange(250.0f, 400.0f);
        float CircleAngle = FMath::RandRange(0.0f, 360.0f);

        // If we have multiple wolves, try to space them around the circle
        if (TotalEnemyCount > 1 && EnemyNumber > 0)
        {
            CircleAngle = (360.0f / TotalEnemyCount) * EnemyNumber + FMath::RandRange(-30.0f, 30.0f);
        }

        // Calculate position on the circle
        FVector CircleOffset = FVector(
            FMath::Cos(FMath::DegreesToRadians(CircleAngle)) * CircleRadius,
            FMath::Sin(FMath::DegreesToRadians(CircleAngle)) * CircleRadius,
            0.0f
        );

        CurrentMoveTarget = TargetPlayer->GetActorLocation() + CircleOffset;

        // Move to the calculated point
        MoveToLocation(CurrentMoveTarget, -1.0f, true, true);
    }
}

void AEnemy_AIController::ApplyDirectMovement(float DeltaTime)
{ // Rock Troll movement - slower, direct approach
    MovementPatternTimer -= DeltaTime;

    if (MovementPatternTimer <= 0.0f)
    {
        // Less frequent updates for slower movement
        MovementPatternTimer = FMath::RandRange(2.0f, 3.0f);

        // Move directly toward player with slight variation
        FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();

        // Small random variation in approach angle
        float RandomAngle = FMath::RandRange(-20.0f, 20.0f);
        FVector RandomDirection = DirectionToPlayer.RotateAngleAxis(RandomAngle, FVector(0, 0, 1));

        // Calculate target with longer distance for slower approach
        float AdvanceDistance = FMath::RandRange(150.0f, 250.0f);
        CurrentMoveTarget = GetPawn()->GetActorLocation() + (RandomDirection * AdvanceDistance);

        // Move to the calculated point with lower acceptance radius
        MoveToLocation(CurrentMoveTarget, 100.0f, true, true);
    }
}

void AEnemy_AIController::PrepareForAttack(EEnemyType TheEnemyType, float DeltaTime)
{// Behavior when close to attack range but not attacking yet
    switch (EnemyType)
    {
    case EEnemyType::Spider:
        // Spiders do quick sidesteps before attacking
    {
        MovementPatternTimer -= DeltaTime;
        if (MovementPatternTimer <= 0.0f)
        {
            MovementPatternTimer = FMath::RandRange(0.3f, 0.7f);

            // Calculate sideways direction relative to player
            FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();
            FVector SideDirection = FVector::CrossProduct(DirectionToPlayer, FVector(0, 0, 1));

            // Randomly select left or right
            if (FMath::RandBool())
            {
                SideDirection = -SideDirection;
            }

            // Calculate sidestep position
            float SideStepDistance = FMath::RandRange(80.0f, 150.0f);
            CurrentMoveTarget = GetPawn()->GetActorLocation() + (SideDirection * SideStepDistance);

            // Move to sidestep position
            MoveToLocation(CurrentMoveTarget, -1.0f, true, true);
        }
    }
    break;

    case EEnemyType::Wolf:
        // Wolves feint forward and back
    {
        MovementPatternTimer -= DeltaTime;
        if (MovementPatternTimer <= 0.0f)
        {
            MovementPatternTimer = FMath::RandRange(0.8f, 1.5f);

            // Calculate direction to player
            FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();

            // Decide between feint forward or back
            float FeintDirection = FMath::RandBool() ? 1.0f : -0.5f; // Forward or back
            float FeintDistance = FMath::RandRange(70.0f, 120.0f) * FeintDirection;

            // Calculate feint position
            CurrentMoveTarget = GetPawn()->GetActorLocation() + (DirectionToPlayer * FeintDistance);

            // Move to feint position
            MoveToLocation(CurrentMoveTarget, -1.0f, true, true);
        }
    }
    break;

    case EEnemyType::RockTroll:
        // Rock Trolls do slow intimidation steps
    {
        MovementPatternTimer -= DeltaTime;
        if (MovementPatternTimer <= 0.0f)
        {
            MovementPatternTimer = FMath::RandRange(1.5f, 2.5f);

            // Calculate slightly offset position from direct approach
            FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();
            float OffsetAngle = FMath::RandRange(-30.0f, 30.0f);
            FVector OffsetDirection = DirectionToPlayer.RotateAngleAxis(OffsetAngle, FVector(0, 0, 1));

            // Calculate step position
            float StepDistance = FMath::RandRange(50.0f, 100.0f);
            CurrentMoveTarget = GetPawn()->GetActorLocation() + (OffsetDirection * StepDistance);

            // Move to step position
            MoveToLocation(CurrentMoveTarget, -1.0f, true, true);
        }
    }
    break;
    }
}

float AEnemy_AIController::GetAttackRateForEnemyType(EEnemyType TheEnemyType)
{
    // Return attack rate multiplier based on enemy type
    switch (EnemyType)
    {
    case EEnemyType::Spider:
        return 1.5f;  // Spiders attack more frequently

    case EEnemyType::Wolf:
        return 1.2f;  // Wolves attack moderately frequently

    case EEnemyType::RockTroll:
        return 0.8f;  // Rock Trolls attack less frequently

    default:
        return 1.0f;
    }
}




void AEnemy_AIController::Tick(float deltaTime)
{
   

        if (bIsFrozen || bIsStunned || bIsAIStopped)
        {
            return; // Skip processing if AI is frozen, stunned, or stopped
        }



        //  UpdateHitState();



        if (TargetPlayer)
        {
            UpdateBehaviour(deltaTime);


        }
        // Ensure the enemy is always facing the player
        FacePlayer();

        
}
