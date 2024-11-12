// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Detection_Arrow.h"
#include "Components/Image.h"


void UEnemy_Detection_Arrow::UpdateArrowRotation(float RotationAngle)
{

    if (ArrowImage)
    {
        FRotator NewRotation = FRotator(0.f, 0.f, RotationAngle);
        ArrowImage->SetRenderTransformAngle(RotationAngle);
    }

}
