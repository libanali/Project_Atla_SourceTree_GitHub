// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "StartUp_Screen_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API AStartUp_Screen_GameMode : public AGameMode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> StartUpScreenWidgetClass;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
