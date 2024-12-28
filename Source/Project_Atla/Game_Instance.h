// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Ren_Low_Poly_Character.h"
#include "Game_Instance.generated.h"

/**
 * 
 */


//class ARen_Low_Poly_Character;


UCLASS()
class PROJECT_ATLA_API UGame_Instance : public UGameInstance
{
	GENERATED_BODY()


public:
    // Store the selected weapon type
    UPROPERTY(BlueprintReadWrite, Category = "Weapons")
        EWeaponType SelectedWeapon;

	
};
