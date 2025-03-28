﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Carousel_Button_Widget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"





void UCarousel_Button_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    
    // Set initial value text
    if (ValueText && PossibleValues.Num() > 0)
    {
        // Make sure CurrentIndex is valid
        if (!PossibleValues.IsValidIndex(CurrentIndex))
        {
            CurrentIndex = 0;
        }
        // Update the text display
        ValueText->SetText(FText::FromString(PossibleValues[CurrentIndex]));
    }

    // Set arrow texts
    if (LeftArrowText)
    {
        LeftArrowText->SetText(FText::FromString(TEXT("<")));
    }
    if (RightArrowText)
    {
        RightArrowText->SetText(FText::FromString(TEXT(">")));
    }

    // Bind button click events
    if (LeftArrowButton)
    {
        LeftArrowButton->OnClicked.AddDynamic(this, &UCarousel_Button_Widget::OnLeftArrowClicked);
    }

    if (RightArrowButton)
    {
        RightArrowButton->OnClicked.AddDynamic(this, &UCarousel_Button_Widget::OnRightArrowClicked);
    }

}



void UCarousel_Button_Widget::CycleValue(bool bNext)
{

    if (PossibleValues.Num() == 0) return;

    if (bNext)
    {
        CurrentIndex = (CurrentIndex + 1) % PossibleValues.Num();
    }
    else
    {
        CurrentIndex = (CurrentIndex - 1 + PossibleValues.Num()) % PossibleValues.Num();
    }

    if (ValueText)
    {
        ValueText->SetText(FText::FromString(PossibleValues[CurrentIndex]));
    }

    // Broadcast value changed event
    OnValueChanged.Broadcast(PossibleValues[CurrentIndex]);

}



FString UCarousel_Button_Widget::GetCurrentValue() const
{

    if (PossibleValues.IsValidIndex(CurrentIndex))
    {
        return PossibleValues[CurrentIndex];
    }
    return FString();
}



void UCarousel_Button_Widget::SetLabel(const FString& NewLabel)
{

    if (LabelText)
    {
        LabelText->SetText(FText::FromString(NewLabel));
    }

}




void UCarousel_Button_Widget::UpdateDisplay()
{


    if (ValueText && PossibleValues.IsValidIndex(CurrentIndex))
    {
        ValueText->SetText(FText::FromString(PossibleValues[CurrentIndex]));
    }



}




void UCarousel_Button_Widget::OnLeftArrowClicked()
{

    CycleValue(false);


}





void UCarousel_Button_Widget::OnRightArrowClicked()
{


    CycleValue(true);


}


/*
FReply UCarousel_Button_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey PressedKey = InKeyEvent.GetKey();

    if (HasKeyboardFocus())
    {
        if (PressedKey == EKeys::Gamepad_DPad_Left || PressedKey == EKeys::A)
        {
            CycleValue(false);
            return FReply::Handled();
        }
        else if (PressedKey == EKeys::Gamepad_DPad_Right || PressedKey == EKeys::D)
        {
            CycleValue(true);
            return FReply::Handled();
        }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

*/


FNavigationReply UCarousel_Button_Widget::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    EUINavigation Direction = InNavigationEvent.GetNavigationType();

    if (Direction == EUINavigation::Left)
    {
        CycleValue(false);
        return FNavigationReply::Explicit(nullptr);
    }
    else if (Direction == EUINavigation::Right)
    {
        CycleValue(true);
        return FNavigationReply::Explicit(nullptr);
    }

    return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
}




