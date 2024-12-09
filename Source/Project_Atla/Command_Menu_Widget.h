// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Command_Menu_Widget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)  // Enum to track the current menu state
enum class ECommandMenuState : uint8
{
    MainMenu UMETA(DisplayName = "Main Menu"),
    ItemsMenu UMETA(DisplayName = "Items Menu"),
    TechniquesMenu UMETA(DisplayName = "Techniques Menu"),
    ElementalAttacks UMETA(DisplayName = "Elemental Attacks")
};



UCLASS()
class PROJECT_ATLA_API UCommand_Menu_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
    // Bind buttons to UI elements in UMG
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* ItemsButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* TechniquesButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* ElementalButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UImage* CommandMenuIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UImage* CommandMenuBackgroundImage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UImage* CommandMenuBackgroundTitleImage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* CommandMenuTitleText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        UWidgetSwitcher* WidgetSwitcher;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
        TSubclassOf<UUserWidget> InventoryWidgetClass;  // This will hold the class type

    UPROPERTY()
        UUserWidget* InventoryWidgetInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CommandMenu")
        TSubclassOf<UUserWidget> TechniquesWidgetClass;

    UPROPERTY()
    UUserWidget* TechniquesWidgetInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CommandMenu")
        TSubclassOf<UUserWidget> ElementalAttacksWidgetClass;



    UPROPERTY()
        UUserWidget* ElementalAttacksWidgetInstance;

    // State variable for tracking the current menu
    ECommandMenuState CurrentMenuState;

    // Native Construct override to set initial bindings
    virtual void NativeOnInitialized() override;

    // Functions to handle the menu transitions
    UFUNCTION()
        void ShowItemsMenu();

    UFUNCTION()
        void ShowTechniquesMenu();

    UFUNCTION()
        void ShowElementalAttacksMenu();

    UFUNCTION()
        void ReturnToMainMenu();

    // Handle back button navigation (Circle / B button)
    UFUNCTION()
        void HandleBackNavigation();

    UFUNCTION()
        void OnItemsButtonClicked();

    UFUNCTION()
        void OnTechniquesButtonClicked();

    UFUNCTION()
        void OnElementalAttacksClicked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
        void CheckInventoryAndSetFocus();


    // Function to show or hide the command menu icon
    void SetCommandMenuIconVisibility(bool bIsVisible);

    void UpdateVisibilityBasedOnIndex(int CurrentIndex);


    virtual void Tick(float DeltaTime);

};
