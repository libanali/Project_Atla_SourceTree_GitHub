// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_AIController.h"
#include "Enemy_Token_Manager.generated.h"

UCLASS()
class PROJECT_ATLA_API AEnemy_Token_Manager : public AActor
{
	GENERATED_BODY()



	
public:	
	// Sets default values for this actor's properties
	AEnemy_Token_Manager();

   
	void RegisterEnemy(AEnemy_AIController* EnemyController);
	void EnemyTurn(AEnemy_AIController* EnemyController);
	void NextTurn();

private:
	TArray<AEnemy_AIController*> EnemyControllers;
	int32 CurrentTurnIndex = 0;


};
