// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_AIController.h"
#include "Enemy_Token_Manager.h"
#include "Kismet/GameplayStatics.h"
#include "Ren_Low_Poly_Character.h"


AEnemy_AIController::AEnemy_AIController()
{
    PrimaryActorTick.bCanEverTick = true;
    bHasAttackToken = false;
    AttackRange = 100.0f;
}

void AEnemy_AIController::BeginPlay()
{
    ARen_Low_Poly_Character* LowPoly_Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (LowPoly_Ren)
    {

        MoveToActor(LowPoly_Ren);

    }

}

void AEnemy_AIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);


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
        {
            AttackPlayer();
        }
        else
        {
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
}
