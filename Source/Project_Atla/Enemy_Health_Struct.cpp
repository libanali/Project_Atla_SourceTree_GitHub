// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Health_Struct.h"


FEnemy_Health_Struct::FEnemy_Health_Struct()
{

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

}

void FEnemy_Health_Struct::InitializeHealth()
{

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

}

