// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elemental_Attack_Type.generated.h"


/**
 * 
 */
 // Define the enum for Elemental Attack Types
UENUM(BlueprintType)
enum class EElementalAttackType : uint8
{
    Fire      UMETA(DisplayName = "Fire"),        // Fire Element
    Ice       UMETA(DisplayName = "Ice"),         // Ice Element
    Thunder   UMETA(DisplayName = "Thunder"),

  //  FireAOE   UMETA(DisplayName = "Fire AOE"),
  //  IceAOE    UMETA(DisplayName = "Ice AOE"),
   // ThunderAOE UMETA(DisplayName = "Thunder AOE"),

  //  FireGround   UMETA(DisplayName = "Fire Ground"),
  //  IceGround    UMETA(DisplayName = "Ice Ground"),
  //  ThunderGround UMETA(DisplayName = "Thunder Ground"),

};