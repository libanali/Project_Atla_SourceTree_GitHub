// Fill out your copyright notice in the Description page of Project Settings.


#include "Mobile_Commands_Widget.h"
#include "Ren_Low_Poly_Character.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"



UMobile_Commands_Widget* UMobile_Commands_Widget::CurrentInstance = nullptr;


void UMobile_Commands_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    CurrentInstance = this;


    // Get player character reference
    PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    bIsVisible = false;
}



void UMobile_Commands_Widget::ShowCommands()
{

    SetVisibility(ESlateVisibility::Visible);
    bIsVisible = true;

    if (PlayerCharacter)
    {
        PlayerCharacter->EnterCommandMode();
    }

}

void UMobile_Commands_Widget::HideCommands()
{

    SetVisibility(ESlateVisibility::Collapsed);
    bIsVisible = false;

    if (PlayerCharacter)
    {
        PlayerCharacter->ExitCommandMode();
    }

}

void UMobile_Commands_Widget::ToggleCommands()
{




}

void UMobile_Commands_Widget::OnCommandExecuted()
{

    if (PlayerCharacter)
    {
        PlayerCharacter->ExitCommandMode();
        PlayerCharacter->SetInputModeForGameplay();
        PlayerCharacter->bIsInUIMode = false;
    }

    // Optionally show the widget if it was hidden
    SetVisibility(ESlateVisibility::Visible);

}


void UMobile_Commands_Widget::NativeDestruct()
{

    if (CurrentInstance == this)
    {
        CurrentInstance = nullptr;
    }
    Super::NativeDestruct();


}