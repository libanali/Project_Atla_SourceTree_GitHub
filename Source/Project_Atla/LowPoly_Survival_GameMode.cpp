// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_Survival_GameMode.h"
#include "Enemy.h"
#include "Ren_Low_Poly_Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


ALowPoly_Survival_GameMode::ALowPoly_Survival_GameMode()
{

    // Set default values
    SpawnDelay = 5.0f;
    EnemiesPerRound = 2;
    SpawnRadius = 4000.0f;
   

}

void ALowPoly_Survival_GameMode::BeginPlay()
{

    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALowPoly_Survival_GameMode::SpawnEnemies, SpawnDelay, true);


}

void ALowPoly_Survival_GameMode::SpawnEnemies()
{


    if (EnemyClass == nullptr) return;

    UWorld* World = GetWorld();
    if (World == nullptr) return;

    for (int32 i = 0; i < EnemiesPerRound; i++)
    {
        //FVector SpawnLocation = FVector(FMath::RandRange(-500, 500), FMath::RandRange(-500, 500), 100);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        World->SpawnActor<AEnemy_Poly>(EnemyClass, GetRandomPointNearPlayer(), SpawnRotation);
    }


}

FVector ALowPoly_Survival_GameMode::GetRandomPointNearPlayer()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController) return FVector::ZeroVector;

    ACharacter* PlayerCharacter = Cast<ARen_Low_Poly_Character>(PlayerController->GetPawn());
    if (!PlayerCharacter) return FVector::ZeroVector;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();

    // Generate a random point around the player within the specified radius
    float RandomAngle = FMath::RandRange(0.f, 360.f);
    float RandomRadius = FMath::RandRange(0.f, SpawnRadius);

    FVector RandomPoint = PlayerLocation + FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
        FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
        0.f);

    return RandomPoint;

    
}
