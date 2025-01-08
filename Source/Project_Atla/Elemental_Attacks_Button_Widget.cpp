// Fill out your copyright notice in the Description page of Project Settings.


#include "Elemental_Attacks_Button_Widget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
//#include "Command_Menu_Widget.h"





void UElemental_Attacks_Button_Widget::NativeOnInitialized()
{

	Super::NativeOnInitialized();


	// Get the player controller (usually the first player controller)
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerController)
	{
		// Set input mode to UI only
		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true; // Show the mouse cursor for UI interaction
	}


	// Bind button click event
	if (Elemental_Attack_Button)
	{
		Elemental_Attack_Button->OnClicked.AddDynamic(this, &UElemental_Attacks_Button_Widget::OnElementalAttackButtonClicked);
		Elemental_Attack_Button->SetKeyboardFocus();
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
		FString ManaCostText = FString::Printf(TEXT("Mana Cost: %.0f"), ElementalAttack.ManaCost);
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

	// Check if the Command Menu Widget and WidgetSwitcher are valid
	if (CommandMenuWidget && CommandMenuWidget->WidgetSwitcher)
	{
		// Set the active widget index to 0
		CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}



