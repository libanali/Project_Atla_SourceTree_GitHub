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
    void FacePlayer();
    void ResetAttackCooldown();
    float DistanceToPlayer;
    float AttackRange;

    // Function to update the behavior of the enemy
    void UpdateBehaviour();

    // Function to set the enemy's assigned number
    void SetEnemyNumber(int32 NewNumber);


    // Function to get the enemy's current assigned number
    int32 GetEnemyNumber() const;

    int32 EnemyNumber;
    bool bIsAttacking;
    bool bIsClosingIn;
    bool bIsInAttackRange;
    bool bIsPetrified;
    FTimerHandle PetrifiedEffectTimer;

 
private:
    AActor* TargetPlayer;
    FTimerHandle AttackDelayHandle;
    bool bIsAttackOnCooldown;  // To check if attack is cooling down
    float LastDecisionTime;  // Tracks the time of the last movement decision
    float DecisionInterval;  // The time interval between decisions
    bool bIsStrafing;        // Tracks if the AI is currently strafing


protected:

    virtual void Tick(float deltaTime);


};
