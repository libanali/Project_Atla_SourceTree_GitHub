// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Base_Item.h"
#include "Inventory.h"
#include "Item_Button_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UItem_Button_Widget : public UUserWidget
{
	GENERATED_BODY()



public:
    virtual void NativeConstruct() override;

    // Bind these widgets in Blueprint
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UButton* Item_Button;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Item_Name;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Item_Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        class UInventory_List_Widget* ParentListWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        FInventoryItem CurrentItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        ABase_Item* Item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class ARen_Low_Poly_Character* PlayerCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Style")
        FSlateBrush HoveredBrush;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
        USoundBase* NavigationSound;


    // Button functions
    UFUNCTION()
        void OnItemButtonClicked();

    UFUNCTION()
        void OnItemButtonHovered();

    UFUNCTION()
        void OnItemButtonUnhovered();


    UFUNCTION(BlueprintImplementableEvent)
        void ReturnToGameplay();

    // Setup functions
    void SetupButton(FInventoryItem ItemData, class ARen_Low_Poly_Character* Character);
    void SetParentList(class UInventory_List_Widget* InParentList);
    void PlayNavigationSound();


    FTimerHandle ItemStateTimer;

protected:
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;



	
};
