// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Command_Menu_Widget.h"
#include "Technique_List_Widget.h"
#include "Technique_Struct.h"
#include "Technique_Button_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UTechnique_Button_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UButton* Technique_Button;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Technique_Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Technique_Points_Required;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
        UCommand_Menu_Widget* CommandMenuWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        UTechnique_List_Widget* ParentListWidget;

    UFUNCTION(BlueprintImplementableEvent)
        void ReturnToGameplay();

    UPROPERTY()
        class ARen_Low_Poly_Character* PlayerCharacter;

    // Setup function
    void SetupButton(FTechnique_Struct TechniqueData, ARen_Low_Poly_Character* Character, int32 Index);

    UFUNCTION()
        void OnTechniqueButtonClicked();

    void SetParentList(UTechnique_List_Widget* InParentList);

    UFUNCTION()
        void OnTechniqueButtonHovered();

    UFUNCTION()
        void OnTechniqueButtonUnhovered();

    UFUNCTION(BlueprintCallable, Category = "UI")
        void SetButtonTextColor(FLinearColor NewColor);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Style")
        FSlateBrush HoveredBrush;

    UPROPERTY()
        FSlateBrush CurrentNormalBrush;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FTechnique_Struct CurrentTechnique;


    UPROPERTY()
        int32 TechniqueIndex;

    void HandleFocusChanged(bool bHasFocus);


protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
};
