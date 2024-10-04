// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy_AIController.generated.h"


class AEnemy_Token_Manager;
/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API AEnemy_AIController : public AAIController
{
    GENERATED_BODY()


public:

    AEnemy_AIController();
    

    virtual void BeginPlay() override;

    void SetTarget(AActor* NewTarget);
    void AttackPlayer();
    void StrafeAroundPlayer();
    void UpdateState();
    void FacePlayer();
    float DistanceToPlayer;
    float AttackRange;
    float StrafeDistance;

    // Function to update the behavior of the enemy
    void UpdateBehavior();

    // Function to set the enemy's assigned number
    void SetEnemyNumber(int32 NewNumber);

    // Function to get the enemy's current assigned number
    int32 GetEnemyNumber() const;
 
private:
    AActor* TargetPlayer;
    bool bIsAttacking;
    int32 EnemyNumber;



protected:

    virtual void Tick(float deltaTime);


};
