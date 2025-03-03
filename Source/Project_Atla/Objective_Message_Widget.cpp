// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective_Message_Widget.h"

void UObjective_Message_Widget::NativeConstruct()
{




}



void UObjective_Message_Widget::PlayShowAnimation()
{

    if (ShowAnimation)
    {
        PlayAnimation(ShowAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward);
    }


}



void UObjective_Message_Widget::PlayHideAnimation()
{


    if (ShowAnimation)
    {
        PlayAnimation(ShowAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse);
    }


}
