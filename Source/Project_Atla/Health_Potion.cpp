// Fill out your copyright notice in the Description page of Project Settings.


#include "Health_Potion.h"
#include "Ren_Low_Poly_Character.h"

UHealth_Potion::UHealth_Potion()
{

	ItemName = "Standard Health Potion";
	MaxStackSize = 10;
	Quantity = 1;

}

void UHealth_Potion::Use(ARen_Low_Poly_Character* Ren)
{

	if (Ren)

	{

		Ren->HealthStruct.IncreaseHealth(2.3f);

	}


}
