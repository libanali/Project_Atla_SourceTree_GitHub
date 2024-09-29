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
 

    //virtual void Tick(float DeltaSeconds) override;

  
    bool bIsAttacking;



private:


public:
    AEnemy_AIController();

protected:
    //virtual void BeginPlay() override;

    // This will store a reference to the player
   


    FTimerHandle TokenReleaseHandle;


  //  class AEnemy_Token_Manager* GetTokenManager();

	
};
