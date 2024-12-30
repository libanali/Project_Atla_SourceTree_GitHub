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

	
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Save Data")
        TMap<EWeaponType, FWeapon_Proficiency_Struct> WeaponProficiencyMap;

    UFUNCTION(BlueprintCallable, Category = "Save Data")
        void LoadPlayerProgress();


    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentAttack;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentDefense;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentMaxHealth;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon Stats")
        float CurrentElementalAttack;





    bool GetWeaponBaseStats(EWeaponType WeaponType, float& OutAttack, float& OutDefense, float& OutElementalAttack) const;
};
