// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LowPoly_Survival_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API ALowPoly_Survival_GameMode : public AGameMode
{
	GENERATED_BODY()



	ALowPoly_Survival_GameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


};
