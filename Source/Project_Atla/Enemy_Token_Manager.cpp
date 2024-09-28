// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Token_Manager.h"
#include "Kismet/GameplayStatics.h"


AEnemy_Token_Manager::AEnemy_Token_Manager()
{
    PrimaryActorTick.bCanEverTick = true;
    MaxAttackers = 1;

}

AEnemy_Token_Manager* AEnemy_Token_Manager::GetTokenManager(UWorld* World)

{
    return Cast<AEnemy_Token_Manager>(UGameplayStatics::GetActorOfClass(World, AEnemy_Token_Manager::StaticClass()));

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
