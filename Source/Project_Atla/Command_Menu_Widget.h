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


UENUM(BlueprintType)
enum class EInputMode : uint8
{
    Gamepad UMETA(DisplayName = "Gamepad"),
    Mouse UMETA(DisplayName = "Mouse")
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

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* InformationText;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* CommandMenuIcon_FadeAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* CommandMenu_FadeAnim;

    // State variable for tracking the current menu
    ECommandMenuState CurrentMenuState;

    FString CurrentText;

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

    UFUNCTION()
        void OnItemsHovered();

    UFUNCTION()
        void OnTechniquesHovered();

    UFUNCTION()
        void OnElementalHovered();


    UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
        void CheckInventoryAndSetFocus();


    UFUNCTION(BlueprintCallable)
        void UpdateInformationText(const FString& NewText);


    // Function to show or hide the command menu icon
    void SetCommandMenuIconVisibility(bool bIsVisible);

    void UpdateVisibilityBasedOnIndex(int CurrentIndex);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    EInputMode CurrentInputMode;





    virtual void Tick(float DeltaTime);

    virtual void NativeOnInitialized() override;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
};
