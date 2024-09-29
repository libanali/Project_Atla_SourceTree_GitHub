// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Token_Manager.h"
#include "Enemy_AIController.h"
#include "Kismet/GameplayStatics.h"


AEnemy_Token_Manager::AEnemy_Token_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

void AEnemy_Token_Manager::RegisterEnemy(AEnemy_AIController* EnemyController)
{
    PrimaryActorTick.bCanEverTick = true;

}

AEnemy_Token_Manager* AEnemy_Token_Manager::GetTokenManager(UWorld* World)

{
    return Cast<AEnemy_Token_Manager>(UGameplayStatics::GetActorOfClass(World, AEnemy_Token_Manager::StaticClass()));

	//EnemyControllers.Add(EnemyController);

}


void AEnemy_Token_Manager::EnemyTurn(AEnemy_AIController* EnemyController)
{

    if (EnemyController)
    {
       // EnemyController->UpdateState();// Call update to handle AI behavior
    }

}

void AEnemy_Token_Manager::NextTurn()
{

    if (EnemyControllers.Num() > 0)
    {
       // EnemyControllers[CurrentTurnIndex]->bIsAttacking = false; // Reset attacking flag
        CurrentTurnIndex = (CurrentTurnIndex + 1) % EnemyControllers.Num(); // Cycle through
        EnemyTurn(EnemyControllers[CurrentTurnIndex]);
    }

}



bool AEnemy_Token_Manager::RequestAttackToken(AEnemy_AIController* AIController)
{
    if (ActiveAttackers.Num() < MaxAttackers)
    {
        ActiveAttackers.Add(AIController);
        return true;
    }


	return false;
}

void AEnemy_Token_Manager::ReleaseAttackToken(AEnemy_AIController* AIController)
{

    ActiveAttackers.Remove(AIController);

}

void AEnemy_Token_Manager::BeginPlay()
{
    Super::BeginPlay();

}

void AEnemy_Token_Manager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
