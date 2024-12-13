// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ElementalEXPManager.generated.h"
/**
 * 
 */


struct FElemental_Struct;


UCLASS()
class PROJECT_ATLA_API UElementalEXPManager : public UObject
{


    GENERATED_BODY()

public:

    // Initialize with a reference to ElementalAttacks
    void InitializeEXPManager(TArray<FElemental_Struct>& InElementalAttacks);

    // Gain EXP for a specific elemental attack
    UFUNCTION(BlueprintCallable)
    void GainEXP(int32 ElementalIndex, float EXPToGain);

    // Check if an elemental attack needs to level up
    void CheckLevelUp(int32 ElementalIndex);


private:
    // Pointer to the ElementalAttacks array
    TArray<FElemental_Struct>* ElementalAttacks;

};
