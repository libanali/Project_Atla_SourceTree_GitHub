// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_AIController.h"
#include "Enemy_Token_Manager.h"
#include "Kismet/GameplayStatics.h"
#include "Ren_Low_Poly_Character.h"

<<<<<<< HEAD
#include "Kismet/GameplayStatics.h"
#include "Enemy_Token_Manager.h"
#include "GameFramework/Character.h"
#include "Enemy_Poly.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
=======
>>>>>>> 5749d8dcf7dd34f86420997710557c35eeb2c13b

AEnemy_AIController::AEnemy_AIController()
{
    PrimaryActorTick.bCanEverTick = true;
<<<<<<< HEAD
    bIsAttacking = false;
    AttackRange = 150.0f;

    // Adjust based on your game needs
=======
    bHasAttackToken = false;
    AttackRange = 100.0f;
>>>>>>> 5749d8dcf7dd34f86420997710557c35eeb2c13b
}

void AEnemy_AIController::BeginPlay()
{
<<<<<<< HEAD
    Super::BeginPlay();

    TargetPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();


}

void AEnemy_AIController::SetTarget(AActor* NewTarget)
{

    TargetPlayer = NewTarget;
=======
    ARen_Low_Poly_Character* LowPoly_Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (LowPoly_Ren)
    {

        MoveToActor(LowPoly_Ren);

    }
>>>>>>> 5749d8dcf7dd34f86420997710557c35eeb2c13b

}

void AEnemy_AIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

<<<<<<< HEAD
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
=======

    if (!PlayerPawn) return;

    float DistanceToPlayer = FVector::Dist(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());

    if (DistanceToPlayer <= AttackRange)
    {
        if (!bHasAttackToken)
        {
            // Request token to attack the player
            RequestAttackToken();
        }

        if (bHasAttackToken)
>>>>>>> 5749d8dcf7dd34f86420997710557c35eeb2c13b
        {
            AttackPlayer();
        }
        else
        {
<<<<<<< HEAD
            StrafeAroundPlayer();
        }
    }

=======
            Strafe();
        }
    }
    else
    {
        MoveToActor(PlayerPawn);
    }
}

void AEnemy_AIController::RequestAttackToken()
{


    AEnemy_Token_Manager* TokenManager = GetTokenManager();

    if (TokenManager && !bHasAttackToken)
    {
        bHasAttackToken = TokenManager->RequestAttackToken(this);

        if (bHasAttackToken)
        {
            // Optionally, set a timer to automatically release the token after attacking for a few seconds
            GetWorld()->GetTimerManager().SetTimer(TokenReleaseHandle, this, &AEnemy_AIController::ReleaseToken, 5.0f, false); // Adjust the delay
        }
    }


}

void AEnemy_AIController::AttackPlayer()
{
}

void AEnemy_AIController::Strafe()
{
}

void AEnemy_AIController::ReleaseToken()
{

    AEnemy_Token_Manager* TokenManager = GetTokenManager();

    if (TokenManager && bHasAttackToken)
    {
        // Release the token and stop attacking
        bHasAttackToken = false;
        TokenManager->ReleaseAttackToken(this);

        // Clear the token release timer
        GetWorld()->GetTimerManager().ClearTimer(TokenReleaseHandle);
    }
}

AEnemy_Token_Manager* AEnemy_AIController::GetTokenManager()
{
     return AEnemy_Token_Manager::GetTokenManager(GetWorld());
>>>>>>> 5749d8dcf7dd34f86420997710557c35eeb2c13b
}
