// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Banner_Widget.h"
#include "Components/TextBlock.h"
#include "Ren_Low_Poly_Character.h"
#include "Components/WidgetComponent.h" 




void UAction_Banner_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    // Set timer to remove widget after 5 seconds
    GetWorld()->GetTimerManager().SetTimer(
        RemoveTimer,
        this,
        &UAction_Banner_Widget::RemoveFromParent_Delayed,
        3.0f,  // 5 seconds
        false  // don't loop
    );



}



void UAction_Banner_Widget::SetText(const FString& Text)
{

    if (ActionText)
    {
        ActionText->SetText(FText::FromString(Text));
    }

}



void UAction_Banner_Widget::RemoveFromParent_Delayed()
{

    // Get the owning character and access its widget component
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (ARen_Low_Poly_Character* Character = Cast<ARen_Low_Poly_Character>(PC->GetPawn()))
        {
            if (Character->ActionBannerWidgetComponent)
            {
                Character->ActionBannerWidgetComponent->SetVisibility(false);
            }
        }
    }
    RemoveFromParent();

}
