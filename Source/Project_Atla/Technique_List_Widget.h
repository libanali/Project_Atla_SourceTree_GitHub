// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ability_Struct.h"
#include "Technique_List_Widget.generated.h"

/**
 * 
 */
 // Forward declare the class we'll be using
class UTechnique_Button_Widget;
class ARen_Low_Poly_Character;


UCLASS()
class PROJECT_ATLA_API UTechnique_List_Widget : public UUserWidget
{
	GENERATED_BODY()


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UScrollBox* Technique_ScrollBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<UTechnique_Button_Widget> TechniqueButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* DescriptionText;


    UPROPERTY()
        ARen_Low_Poly_Character* PlayerCharacter;



    void SetupWidget(ARen_Low_Poly_Character* Character);
    void PopulateTechniqueList();
    void SetupInputMode();


protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    bool IsControllerConnected() const;


    UFUNCTION(BlueprintCallable)
    bool IsRunningOnMobile() const;

};
