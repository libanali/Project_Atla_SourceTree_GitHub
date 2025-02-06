// Fill out your copyright notice in the Description page of Project Settings.


#include "Carousel_Button_Widget.h"
#include "Components/TextBlock.h"





void UCarousel_Button_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    
    // Set initial value text
    if (ValueText && PossibleValues.Num() > 0)
    {
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




