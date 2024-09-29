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

<<<<<<< HEAD
   
	void RegisterEnemy(AEnemy_AIController* EnemyController);
	void EnemyTurn(AEnemy_AIController* EnemyController);
	void NextTurn();

private:
	TArray<AEnemy_AIController*> EnemyControllers;
	int32 CurrentTurnIndex = 0;

=======
	// Maximum number of enemies that can attack at once
	int32 MaxAttackers;

	// Store references to AI controllers with tokens
	TArray<AEnemy_AIController*> ActiveAttackers;

	// Singleton instance to easily get token manager
	static AEnemy_Token_Manager* GetTokenManager(UWorld* World);

	// Request and release token functions
	bool RequestAttackToken(AEnemy_AIController* AIController);
	void ReleaseAttackToken(AEnemy_AIController* AIController);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
>>>>>>> 5749d8dcf7dd34f86420997710557c35eeb2c13b

};
