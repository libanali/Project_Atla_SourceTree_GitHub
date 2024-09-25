// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	// Grant a token to an enemy if there are available tokens
	bool GrantToken(AEnemy_Poly* Enemy);

	// Revoke a token when an enemy finishes attacking
	void RevokeToken(AEnemy_Poly* Enemy);

	// Check if an enemy currently holds a token
	bool HasToken(AEnemy_Poly* Enemy) const;

	// Set the maximum number of tokens (can change per wave)
	void SetMaxTokens(int32 NewMaxTokens);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
