// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Poly_HealthBar_Widget.h"
#include "Components/ProgressBar.h"

void UEnemy_Poly_HealthBar_Widget::UpdateHealthBar(float HealthPercentage)
{


    if (HealthProgressBar) // Ensure the progress bar is valid
    {
        // Update the progress bar's percentage
        HealthProgressBar->SetPercent(FMath::Clamp(HealthPercentage, 0.0f, 1.0f));
    }


}
