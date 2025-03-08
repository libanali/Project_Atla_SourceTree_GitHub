// Fill out your copyright notice in the Description page of Project Settings.


#include "ATB_Struct.h"
#include "Ren_Character.h"

FATB_Struct::FATB_Struct()
{
}

void FATB_Struct::InitialiseATB()
{

	CurrentATB = 0.0f;
	MaxATB = 100.0f;
	ATBFillRate = 3.0f;
	ATB_Attack_Boost = 1.2f;

}




