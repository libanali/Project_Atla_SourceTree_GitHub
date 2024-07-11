// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Controller.h"
#include "LowPoly_AI_Controller.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API ALowPoly_AI_Controller : public AAIController
{
	GENERATED_BODY()

public:
		ALowPoly_AI_Controller();

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn);


	void AttackPlayer();
	void MoveToPlayer();
	bool CanAttack() const;


	static void AddEnemy(ALowPoly_AI_Controller* Enemy);
	static void RemoveEnemy(ALowPoly_AI_Controller* Enemy);
	static void AssignNextToken();


private:
	static TArray<ALowPoly_AI_Controller*> Enemies;
	static int32 CurrentTokenIndex;
	bool bHasToken;

};
