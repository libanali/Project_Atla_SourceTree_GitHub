// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Arrow_Widget.h"
#include "Components/Image.h"



UEnemy_Arrow_Widget::UEnemy_Arrow_Widget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{


}

void UEnemy_Arrow_Widget::UpdateArrowRotation(float Angle)
{

	SetArrowRotation(Angle);

}

void UEnemy_Arrow_Widget::NativeConstruct()
{

	Super::NativeConstruct();

}

void UEnemy_Arrow_Widget::SetArrowRotation(float RotationAngle)
{

    // Set the rotation of the ArrowImage widget
    if (ArrowImage)
    {
        ArrowImage->SetRenderTransformAngle(RotationAngle);
    }

}
