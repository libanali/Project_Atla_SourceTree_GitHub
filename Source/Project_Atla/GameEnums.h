// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameEnums.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Sword UMETA(DisplayName = "Sword"),
	Staff UMETA(DisplayName = "Staff")
};

UENUM(BlueprintType)
enum class EQueuedActionType : uint8
{
    None,
    Technique,
    Ability,
    Elemental,
    Item,
    PowerUp,
    Attack
};
