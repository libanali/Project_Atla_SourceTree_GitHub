// Fill out your copyright notice in the Description page of Project Settings.


#include "Health_Struct.h"


FHealth_Struct::FHealth_Struct()
{

}

void FHealth_Struct::InitializeHealth()
{

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

}


void FHealth_Struct::TakeDamage(float DamageAmount)
{

	CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.0f);

}

void FHealth_Struct::IncreaseHealth(float HealAmount)
{

	CurrentHealth = FMath::Min(CurrentHealth * HealAmount, MaxHealth);

}


