// Fill out your copyright notice in the Description page of Project Settings.


#include "Super_Health_Potion.h"
#include "Ren_Low_Poly_Character.h"

void USuper_Health_Potion::UseItem(ARen_Low_Poly_Character* Ren)
{

	if (Ren)

	{

		float HealthIncrease = Ren->HealthStruct.CurrentHealth * HealthIncreasePercentage;
		Ren->HealthStruct.IncreaseHealth(HealthIncreasePercentage);
	}

}
