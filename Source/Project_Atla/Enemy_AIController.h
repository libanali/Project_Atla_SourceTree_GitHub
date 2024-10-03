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
    bool HasToken;
 
private:
    AActor* TargetPlayer;
    bool bIsAttacking = false;
    AEnemy_Token_Manager* TokenManager;


protected:

    virtual void Tick(float deltaTime);


};
