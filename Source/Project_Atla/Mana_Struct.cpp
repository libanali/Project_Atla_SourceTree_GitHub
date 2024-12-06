// Fill out your copyright notice in the Description page of Project Settings.


#include "Mana_Struct.h"

FMana_Struct::FMana_Struct()
{



}

void FMana_Struct::InitializeMana()
{

	MaxMana = 70.0f;
	CurrentMana = MaxMana;

}


void FMana_Struct::IncreaseMana(float ManaAmount)
{

	CurrentMana = FMath::Min(CurrentMana + ManaAmount, MaxMana);

}




void FMana_Struct::DecreaseMana(float DecreaseAmount)
{

	CurrentMana = FMath::Max(CurrentMana - DecreaseAmount, 0.0f);



}
