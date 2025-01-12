// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Elemental_Struct.h" // Include your elemental struct
#include "Elemental_Attacks_List_Widget.generated.h"

/**
 * 
 */

 // Forward declarations
class UElemental_Attacks_Button_Widget;
class ARen_Low_Poly_Character;



UCLASS()
class PROJECT_ATLA_API UElemental_Attacks_List_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
    /** Initializes the widget and populates the list of elemental attacks */
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;

    void SetupInputMode();

    /** Set up the widget with a reference to the player character */
    UFUNCTION(BlueprintCallable)
    void SetupWidget(ARen_Low_Poly_Character* Character);


    /** The widget class for the elemental attack button */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<UElemental_Attacks_Button_Widget> ElementalAttackButtonClass;



private:
    /** Populates the scroll box with buttons for unlocked elemental attacks */

    void PopulateElementalAttackList();
    void NewPopulateElementalAttackList();


    /** Reference to the scroll box container for the buttons */
    UPROPERTY(meta = (BindWidget))
       class UScrollBox* Elemental_Attack_ScrollBox;

    /** Reference to the player character */
    ARen_Low_Poly_Character* PlayerCharacter;


    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

};
