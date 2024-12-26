// Fill out your copyright notice in the Description page of Project Settings.


#include "Main_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"


void UMain_Menu_Widget::NativeConstruct()
{

    // Bind buttons to functions
    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnPlayClicked);
    }
    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UMain_Menu_Widget::OnBackClicked);
    }



    if (PressAnyButtonText)

    {
        PlayAnimation(FadeAnimation, 1.0f, 0);
    }


}


void UMain_Menu_Widget::OnPlayClicked()
{

    if (WidgetSwitcher)
    {
        // Switch to Weapon Select (index 2)
        WidgetSwitcher->SetActiveWidgetIndex(2);
    }


}

void UMain_Menu_Widget::OnBackClicked()
{

    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        // Logic to go back to the previous menu
        if (CurrentIndex > 0)
        {
            WidgetSwitcher->SetActiveWidgetIndex(CurrentIndex - 1);
        }
    }



}

void UMain_Menu_Widget::HandleGoBack()
{


    if (WidgetSwitcher)
    {
        int32 CurrentIndex = WidgetSwitcher->GetActiveWidgetIndex();

        // Logic to go back to the previous menu
        if (CurrentIndex > 0)
        {
            WidgetSwitcher->SetActiveWidgetIndex(CurrentIndex - 1);
        }
    }



}
