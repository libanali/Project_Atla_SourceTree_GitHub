// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Main_Menu_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API AMain_Menu_PlayerController : public APlayerController
{
	GENERATED_BODY()



protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

public:
    // Back button functionality
    void GoBack();


	
};
