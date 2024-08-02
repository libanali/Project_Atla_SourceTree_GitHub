// Fill out your copyright notice in the Description page of Project Settings.


#include "Defence_Potion.h"
#include "Ren_Low_Poly_Character.h"

void UDefence_Potion::UseItem(ARen_Low_Poly_Character* Ren)
{

	if (Ren)

	{

		Ren->IncreaseDefence(DefenseIncreasePercentage, Duration);

	}


}
