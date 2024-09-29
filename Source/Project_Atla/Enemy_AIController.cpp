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
        // Implement attack logic here (e.g., call the attack function from Enemy_Poly)
        AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());

        if (Enemy)
        {
            Enemy->Attack();  // Call the attack logic from the Enemy class
            GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Attack!"));

            bIsAttacking = true;

            // Set a timer to reset bIsAttacking after 2 seconds (or however long you want the delay)
            GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
                {
                    bIsAttacking = false;
                });
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
        float DistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());
        if (DistanceToPlayer < AttackRange) // Define AttackRange in your Enemy_Poly class
        {
            AttackPlayer();
        }
        else
        {
            StrafeAroundPlayer();
        }
    }


}

void AEnemy_AIController::Tick(float deltaTime)
{
    // Ensure there is a valid player target
    if (TargetPlayer)
    {
        float DistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());
        GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Yellow, FString::Printf(TEXT("Distance to player: %f"), DistanceToPlayer));

        if (DistanceToPlayer <= AttackRange)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Within Attack Range!"));
            if (!bIsAttacking)
            {
                AttackPlayer();
            }
        }
        else
        {
            MoveToActor(TargetPlayer, 50.0f);
        }
    }

}
