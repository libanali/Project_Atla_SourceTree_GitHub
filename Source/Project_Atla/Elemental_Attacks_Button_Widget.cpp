// Fill out your copyright notice in the Description page of Project Settings.


#include "Elemental_Attacks_Button_Widget.h"
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

    // Just bind the click - keep it simple
    Elemental_Attack_Button->OnClicked.AddDynamic(this, &UElemental_Attacks_Button_Widget::OnElementalAttackButtonClicked);
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





void UElemental_Attacks_Button_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

 
}








void UElemental_Attacks_Button_Widget::SetupButton(FElemental_Struct ElementalAttack, ARen_Low_Poly_Character* Character, int32 Index)
{
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




