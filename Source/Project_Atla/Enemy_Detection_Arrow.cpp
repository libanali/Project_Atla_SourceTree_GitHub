// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Detection_Arrow.h"
#include "Components/Image.h"


void UEnemy_Detection_Arrow::NativeConstruct()
{

    Super::NativeConstruct();

    SetVisibility(ESlateVisibility::Hidden);


}

void UEnemy_Detection_Arrow::UpdateArrowRotation(float RotationAngle)
{

    if (ArrowImage)
    {
        float ScreenAngle = RotationAngle + 90.0f; // Add 90 degrees to align arrow properly
        ArrowImage->SetRenderTransformAngle(ScreenAngle);
    }

}
