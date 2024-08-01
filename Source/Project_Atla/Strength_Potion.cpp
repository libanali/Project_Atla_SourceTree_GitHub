// Fill out your copyright notice in the Description page of Project Settings.


#include "Strength_Potion.h"
#include "Ren_Low_Poly_Character.h"

void UStrength_Potion::Use(ARen_Low_Poly_Character* Ren)
{

    if (Ren)
    {
        Ren->IncreaseAttack(StrengthIncreasePercentage, Duration);
    }


}
