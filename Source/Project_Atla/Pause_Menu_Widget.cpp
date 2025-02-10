// Fill out your copyright notice in the Description page of Project Settings.


#include "Pause_Menu_Widget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Components/WidgetSwitcher.h"

void UPause_Menu_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button click events
    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnResumeClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnQuitClicked);
    }

    if (ConfirmYesButton)
    {
        ConfirmYesButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitYes);
    }

    if (ConfirmNoButton)
    {
        ConfirmNoButton->OnClicked.AddDynamic(this, &UPause_Menu_Widget::OnConfirmQuitNo);
    }

    // Initially hide the confirm quit canvas
    if (ConfirmQuitCanvas)
    {
        ConfirmQuitCanvas->SetVisibility(ESlateVisibility::Hidden);
    }

    // Set initial widget switcher index to main menu
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);  // Assuming MainPauseCanvas is the first widget
    }
}


void UPause_Menu_Widget::ShowPauseMenu()
{

    AddToViewport();

    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);  // Show main menu
        if (MainMenuAnimation)
        {
            PlayAnimation(MainMenuAnimation);
        }
    }

    // Show mouse cursor and set input mode
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetShowMouseCursor(true);
        PlayerController->SetInputMode(FInputModeUIOnly());
    }

}



void UPause_Menu_Widget::HidePauseMenu()
{

    RemoveFromParent();

    // Hide mouse cursor and reset input mode
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetShowMouseCursor(false);
        PlayerController->SetInputMode(FInputModeGameOnly());
    }



}




void UPause_Menu_Widget::OnResumeClicked()
{
    HidePauseMenu();

}




void UPause_Menu_Widget::OnQuitClicked()
{

    if (MainPauseCanvas && ConfirmQuitCanvas)
    {
        MainPauseCanvas->SetVisibility(ESlateVisibility::Hidden);
        ConfirmQuitCanvas->SetVisibility(ESlateVisibility::Visible);

        if (ConfirmMenuAnimation)
        {
            PlayAnimation(ConfirmMenuAnimation);
        }
    }


}



void UPause_Menu_Widget::OnConfirmQuitYes()
{





}




void UPause_Menu_Widget::OnConfirmQuitNo()
{






}








