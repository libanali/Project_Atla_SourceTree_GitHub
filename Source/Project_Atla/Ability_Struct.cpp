// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Struct.h"


FAbility_Struct::FAbility_Struct()
{
}

void FAbility_Struct::InitializeAbilityPoints()
{

	CurrentAbilityPoints = 0.0f;
	CurrentAbilityPoints = MaxAbilityPoints;


}

void FAbility_Struct::IncreaseAbilityPoints(float AbilityPointsAmount)
{

	CurrentAbilityPoints = FMath::Min(CurrentAbilityPoints + AbilityPointsAmount, MaxAbilityPoints);

}

