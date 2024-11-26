// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Player_Save_Game.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UPlayer_Save_Game : public USaveGame
{
	GENERATED_BODY()
	
public:

    // High scores
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "High Score")
        int32 SwordHighScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "High Score")
        int32 StaffHighScore;



    // Constructor to initialize defaults (optional)
    UPlayer_Save_Game()
        : SwordHighScore(0), StaffHighScore(0) {}

};
