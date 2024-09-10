// Fill out your copyright notice in the Description page of Project Settings.


#include "Super_Health_Potion.h"
#include "Ren_Low_Poly_Character.h"

USuper_Health_Potion::USuper_Health_Potion()
{
	ItemName = "Super Health Potion";
	MaxStackSize = 10;
	Quantity = 1;

}

void USuper_Health_Potion::Use(ARen_Low_Poly_Character* Ren)
{

	if (Ren)

	{

		Ren->HealthStruct.IncreaseHealth(4.3f);

	}


}
