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

protected:
    virtual void BeginPlay() override;

    // This will store a reference to the player
    APawn* PlayerPawn;

    bool bHasAttackToken;
    float AttackRange;

public:
    // Called every frame to update AI decisions
    virtual void Tick(float DeltaSeconds) override;

    // Functions for attack and strafing behavior
    void RequestAttackToken();
    void AttackPlayer();
    void Strafe();
    void ReleaseToken();

    FTimerHandle TokenReleaseHandle;


    class AEnemy_Token_Manager* GetTokenManager();

	
};
