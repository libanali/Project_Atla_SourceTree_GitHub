// Fill out your copyright notice in the Description page of Project Settings.


#include "Health_Potion.h"
#include "Ren_Low_Poly_Character.h"

void UHealth_Potion::UseItem(ARen_Low_Poly_Character* Ren)
{


    if (Ren)
    {
        float HealthIncrease = Ren->HealthStruct.CurrentHealth * HealthIncreasePercentage;
        Ren->HealthStruct.IncreaseHealth(HealthIncrease);
    }


}
