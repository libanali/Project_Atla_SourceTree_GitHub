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

private:
	int32 MaxTokens; //maximum number of enemies allows to attack at once
	TArray<class AEnemy_Poly*> ActiveEnemies; //Enemies currently holding tokens

	
public:	
	// Sets default values for this actor's properties
	AEnemy_Token_Manager();

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

};
