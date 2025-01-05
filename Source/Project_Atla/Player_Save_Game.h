// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Weapon_Proficiency_Struct.h"  // Include the struct definition
#include "Elemental_Proficiency_Struct.h"
#include "Ren_Low_Poly_Character.h"
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

    // Map to store weapon proficiency for different weapon types
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Proficiency")
        TMap<EWeaponType, FWeapon_Proficiency_Struct> SavedWeaponProficiencyMap;

    UPROPERTY(BlueprintReadWrite, Category = "Elemental Proficiency")
        TMap<EWeaponType, FElemental_Proficiency_Struct> SavedElementalProficiencyMap;

   // UPROPERTY(BlueprintReadWrite, Category = "Elemental Attacks")
     //   TArray<FElemental_Struct> SavedElementalAttacks;

    UPROPERTY(BlueprintReadWrite, Category = "Elemental Attacks") 
        TMap<EWeaponType, FWeaponElementalAttacks> SavedWeaponElementalAttacks;

    UPROPERTY(BlueprintReadWrite, Category = "Elemental Usage")
        float FireElementalUsage;

    UPROPERTY(BlueprintReadWrite, Category = "Elemental Usage")
        float IceElementalUsage;

    UPROPERTY(BlueprintReadWrite, Category = "Elemental Usage")
        float ThunderElementalUsage;

    // Constructor to initialize defaults (optional)
    UPlayer_Save_Game()
        : SwordHighScore(0), StaffHighScore(0) {}

};
