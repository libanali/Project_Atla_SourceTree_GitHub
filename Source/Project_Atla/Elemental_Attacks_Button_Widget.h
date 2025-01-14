// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Elemental_Struct.h"
#include "Elemental_Struct.h"
#include "Elemental_Attacks_List_Widget.h"
#include "Command_Menu_Widget.h"
#include "Elemental_Attacks_Button_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UElemental_Attacks_Button_Widget : public UUserWidget
{
	GENERATED_BODY()

public:

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Elemental_Attack_Button;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* Elemental_Attack_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* Elemental_Attack_ManaCost;

	// Reference to the Command Menu Widget
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
		UCommand_Menu_Widget* CommandMenuWidget;

	UPROPERTY()
		UElemental_Attacks_List_Widget* ParentListWidget;


	UFUNCTION(BlueprintImplementableEvent)
		 void ReturnToGameplay();

	UPROPERTY()
		class ARen_Low_Poly_Character* PlayerCharacter;

	// Function to initialize the button with the attack data
	void SetupButton(FElemental_Struct ElementalAttack, ARen_Low_Poly_Character* Character, int32 Index);

	// Function to handle button click
	UFUNCTION()
		void OnElementalAttackButtonClicked();
	
	int32 ElementalIndex;

	void OnAnyButtonClicked();

	void SetParentList(UElemental_Attacks_List_Widget* InParentList);

	UFUNCTION()
		void OnElementalAttackButtonHovered();

	UFUNCTION()
		void OnElementalAttackButtonUnhovered();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Style")
		FSlateBrush HoveredBrush;

	UPROPERTY()
		FSlateBrush CurrentNormalBrush;

	UPROPERTY()  // Add this property
		FElemental_Struct CurrentElementalAttack;

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
};
