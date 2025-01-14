// Fill out your copyright notice in the Description page of Project Settings.


#include "Elemental_Attacks_Button_Widget.h"
#include "Elemental_Attacks_List_Widget.h"
#include "Elemental_Struct.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"




void UElemental_Attacks_Button_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!Elemental_Attack_Button)
    {
        UE_LOG(LogTemp, Warning, TEXT("Elemental_Attack_Button is nullptr!"));
        return;
    }


    // Clear existing delegates before adding new ones
    Elemental_Attack_Button->OnClicked.Clear();
    Elemental_Attack_Button->OnHovered.Clear();
    Elemental_Attack_Button->OnUnhovered.Clear();


    // Store the original normal brush
    CurrentNormalBrush = Elemental_Attack_Button->WidgetStyle.Normal;

    // Set up button style for hover/focus states
    FButtonStyle ButtonStyle = Elemental_Attack_Button->WidgetStyle;
    ButtonStyle.SetHovered(HoveredBrush);
    ButtonStyle.SetPressed(HoveredBrush);

    // Apply the style
    Elemental_Attack_Button->SetStyle(ButtonStyle);

    // Rest of your setup...
    Elemental_Attack_Button->OnClicked.AddDynamic(this, &UElemental_Attacks_Button_Widget::OnElementalAttackButtonClicked);
    Elemental_Attack_Button->OnHovered.AddDynamic(this, &UElemental_Attacks_Button_Widget::OnElementalAttackButtonHovered);
    Elemental_Attack_Button->OnUnhovered.AddDynamic(this, &UElemental_Attacks_Button_Widget::OnElementalAttackButtonUnhovered);

    Elemental_Attack_Button->SetIsEnabled(true);
    Elemental_Attack_Button->IsFocusable = true;

 
}




void UElemental_Attacks_Button_Widget::NativeOnInitialized()
{

    Super::NativeOnInitialized();

/*
    if (Elemental_Attack_Button)
    {

        Elemental_Attack_Button->OnClicked.AddDynamic(this, &UElemental_Attacks_Button_Widget::OnElementalAttackButtonClicked);

    }

    bIsFocusable = true;

    */
}





void UElemental_Attacks_Button_Widget::OnAnyButtonClicked()
{

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Button clicked!"));


}

void UElemental_Attacks_Button_Widget::SetParentList(UElemental_Attacks_List_Widget* InParentList)
{

    ParentListWidget = InParentList;

}



void UElemental_Attacks_Button_Widget::OnElementalAttackButtonHovered()
{

    if (ParentListWidget && ParentListWidget->DescriptionText)
    {
        ParentListWidget->DescriptionText->SetText(FText::FromString(CurrentElementalAttack.ElementDescription));
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Hover Description Updated"));
    }

}

void UElemental_Attacks_Button_Widget::OnElementalAttackButtonUnhovered()
{

    if (ParentListWidget && ParentListWidget->DescriptionText)
    {
        ParentListWidget->DescriptionText->SetText(FText::GetEmpty());
    }


}






void UElemental_Attacks_Button_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

 
}








void UElemental_Attacks_Button_Widget::SetupButton(FElemental_Struct ElementalAttack, ARen_Low_Poly_Character* Character, int32 Index)
{


    // Store the elemental attack data
    CurrentElementalAttack = ElementalAttack;

    // Set up the button text for attack name and mana cost
    if (Elemental_Attack_Name)
    {
        Elemental_Attack_Name->SetText(FText::FromString(ElementalAttack.ElementalAttackName));
    }

    if (Elemental_Attack_ManaCost)
    {
        FString ManaCostText = FString::Printf(TEXT("%.0f"), ElementalAttack.ManaCost);
        Elemental_Attack_ManaCost->SetText(FText::FromString(ManaCostText));
    }

    PlayerCharacter = Character;
    ElementalIndex = Index;


    // Debug logging to check parent list
    if (!ParentListWidget)
    {
        // Try to find the parent list widget
        ParentListWidget = Cast<UElemental_Attacks_List_Widget>(GetParent());

        if (!ParentListWidget)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
                TEXT("Could not find ParentListWidget in SetupButton"));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                TEXT("ParentListWidget found successfully"));
        }

    }
}



FReply UElemental_Attacks_Button_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

    if (InKeyEvent.GetKey() == EKeys::Enter ||
        InKeyEvent.GetKey() == EKeys::SpaceBar ||
        InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
    {
        if (Elemental_Attack_Button)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Key pressed on elemental button!"));
            OnElementalAttackButtonClicked();
            return FReply::Handled();
        }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

}



FReply UElemental_Attacks_Button_Widget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{


    if (Elemental_Attack_Button)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Focus Received - Setting Hovered Brush"));

        FButtonStyle ButtonStyle = Elemental_Attack_Button->WidgetStyle;
        ButtonStyle.SetNormal(HoveredBrush);
        Elemental_Attack_Button->SetStyle(ButtonStyle);
        

        if (ParentListWidget && ParentListWidget->DescriptionText)
        {
            ParentListWidget->DescriptionText->SetText(FText::FromString(CurrentElementalAttack.ElementDescription));
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Focus Description Updated"));
        }

        else

        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Focus Description Not Updated"));


        }
       
    }


    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);


}



void UElemental_Attacks_Button_Widget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{

    if (Elemental_Attack_Button)
    {
        FButtonStyle ButtonStyle = Elemental_Attack_Button->WidgetStyle;
        ButtonStyle.SetNormal(CurrentNormalBrush);  // Use the stored original brush
        Elemental_Attack_Button->SetStyle(ButtonStyle);


        // Clear the description when focus is lost
        if (ParentListWidget && ParentListWidget->DescriptionText)
        {
            ParentListWidget->DescriptionText->SetText(FText::GetEmpty());
        }
    }

    Super::NativeOnFocusLost(InFocusEvent);

}



void UElemental_Attacks_Button_Widget::OnElementalAttackButtonClicked()
{
    // Ensure the player character is valid and the index is valid
    if (PlayerCharacter && ElementalIndex >= 0)
    {
        PlayerCharacter->UseElementalAttack(ElementalIndex); // Call the player's attack function
        ReturnToGameplay();
    }


    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Elemental Attack Button Clicked!"));
}




