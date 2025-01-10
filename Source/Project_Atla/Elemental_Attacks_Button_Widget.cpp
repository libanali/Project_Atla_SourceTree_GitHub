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


    SetFocusTimer();
}




void UElemental_Attacks_Button_Widget::NativeOnInitialized()
{

    Super::NativeOnInitialized();


    if (Elemental_Attack_Button)
    {

        Elemental_Attack_Button->OnClicked.AddDynamic(this, &UElemental_Attacks_Button_Widget::OnElementalAttackButtonClicked);

    }


}




void UElemental_Attacks_Button_Widget::SetFocusTimer()
{

    FTimerManager& TimerManager = GetWorld()->GetTimerManager();

    FTimerHandle FocusTimerHandle;
    TimerManager.SetTimer(FocusTimerHandle, this, &UElemental_Attacks_Button_Widget::FocusOnButton, 2.1f, false);

}



void UElemental_Attacks_Button_Widget::FocusOnButton()
{

    Elemental_Attack_Button->SetKeyboardFocus();

    if (PlayerCharacter && PlayerCharacter->bIsElementalsOpen)

    {

         GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, TEXT("Elemental list is open!"));
          Elemental_Attack_Button->SetKeyboardFocus();

    }

  
    else

    {

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Elemental button doesn't have keyboard focus"));

    }
}




void UElemental_Attacks_Button_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (Elemental_Attack_Button->HasKeyboardFocus())
    {
      //  GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, TEXT("Elemental button has keyboard focus"));
    }

    if (PlayerCharacter && PlayerCharacter->bIsElementalsOpen)

    {

      //  GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, TEXT("Elemental list is open!"));
        //Elemental_Attack_Button->SetKeyboardFocus();

    }

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




void UElemental_Attacks_Button_Widget::OnElementalAttackButtonClicked()
{
    // Ensure the player character is valid and the index is valid
    if (PlayerCharacter && ElementalIndex >= 0)
    {
        PlayerCharacter->UseElementalAttack(ElementalIndex); // Call the player's attack function
    }


    UE_LOG(LogTemp, Warning, TEXT("Clicked!"))
}




