// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_AIController.h"

#include "Kismet/GameplayStatics.h"
#include "Enemy_Token_Manager.h"
#include "GameFramework/Character.h"
#include "Enemy_Poly.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy_AIController::AEnemy_AIController()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsAttacking = false;
    AttackRange = 150.0f;

    // Adjust based on your game needs
}

void AEnemy_AIController::BeginPlay()
{
    Super::BeginPlay();

    TargetPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();


}

void AEnemy_AIController::SetTarget(AActor* NewTarget)
{

    TargetPlayer = NewTarget;

}

void AEnemy_AIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}


void AEnemy_AIController::AttackPlayer()
{

    if (TargetPlayer)
    {
        // Implement attack logic here (e.g., call the attack function from Enemy_Poly)
        AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());
        if (Enemy)
        {
           Enemy->Attack(); // Assuming UseItem() is the attack function
        }
        bIsAttacking = true;
    }

}

void AEnemy_AIController::StrafeAroundPlayer()
{

    if (TargetPlayer)
    {
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
        if (DistanceToPlayer < AttackRange) 
        {
            AttackPlayer();
        }
        else
        {
            StrafeAroundPlayer();
        }
    }

}
