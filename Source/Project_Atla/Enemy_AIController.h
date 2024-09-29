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

<<<<<<< HEAD

public:
    AEnemy_AIController();

    virtual void Tick(float DeltaSeconds) override;

    void UpdateState();
    bool bIsAttacking;



protected:
    virtual void BeginPlay() override;
    void SetTarget(AActor* NewTarget);
    void AttackPlayer();
    void StrafeAroundPlayer();

private:
    AActor* TargetPlayer;
    float AttackRange;
=======
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

>>>>>>> 5749d8dcf7dd34f86420997710557c35eeb2c13b
	
};
