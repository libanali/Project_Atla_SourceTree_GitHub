// Fill out your copyright notice in the Description page of Project Settings.


#include "Round_Complete_Message_Widget.h"

void URound_Complete_Message_Widget::PlayShowAnimation()
{

    if (ShowAnimation)
    {
        PlayAnimation(ShowAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward);
    }

}

void URound_Complete_Message_Widget::PlayHideAnimation()
{

    if (ShowAnimation)
    {
        PlayAnimation(ShowAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse);
    }

}
