// Fill out your copyright notice in the Description page of Project Settings.


#include "Test_Button_Widget.h"
#include "Components/Button.h"

void UTest_Button_Widget::NativeOnInitialized()
{
}

void UTest_Button_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    bIsFocusable = true;

    if (TestButton)
    {
        TestButton->SetIsEnabled(true);
        TestButton->IsFocusable = true;
      //  TestButton->SetKeyboardFocus();
        TestButton->OnClicked.AddDynamic(this, &UTest_Button_Widget::OnButtonClicked);

    }

}



FReply UTest_Button_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
     if (InKeyEvent.GetKey() == EKeys::Enter || 
        InKeyEvent.GetKey() == EKeys::SpaceBar || 
        InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
    {
        if (TestButton)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Key pressed on button!"));
            OnButtonClicked();  // Directly call OnButtonClicked instead of broadcasting
            return FReply::Handled();
        }
    }
    
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}



void UTest_Button_Widget::OnButtonClicked()
{

    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Test Button Clicked!"));


}
