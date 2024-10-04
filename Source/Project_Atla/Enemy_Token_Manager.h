// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_AIController.h"
#include "Enemy_Token_Manager.generated.h"

class AEnemy_AIController;

UCLASS()
class PROJECT_ATLA_API AEnemy_Token_Manager : public AActor
{
	GENERATED_BODY()



	
public:	

	AEnemy_Token_Manager();

	// Function to register an enemy in the system
	void RegisterEnemy(AEnemy_AIController* EnemyController);

	// Function to advance to the next turn
	void NextTurn();

	// Call to start the token system
	void StartTokenSystem();

	void GiveToken(AEnemy_AIController* EnemyController);
	void EndTurn(AEnemy_AIController* EnemyController);

private:

	// Array of enemy AI controllers
	TArray<AEnemy_AIController*> EnemyControllers;

	// The index of the current enemy whose turn it is to attack
	int32 CurrentEnemyIndex;

	// Timer handle for managing turn duration
	FTimerHandle TurnTimerHandle;

	// Delay between each enemy's turn
	float TurnDuration; // Adjust as needed

	// Function called every time a turn ends to move to the next enemy
	void HandleNextTurn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
