// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy_AIController.generated.h"

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

private:
    AActor* TargetPlayer;
    bool bIsAttacking = false;
    float AttackRange;

protected:

    virtual void Tick(float deltaTime);


};
