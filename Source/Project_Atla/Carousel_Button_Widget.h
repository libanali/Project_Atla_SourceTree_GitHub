// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Carousel_Button_Widget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueChanged, const FString&, NewValue);


UCLASS()
class PROJECT_ATLA_API UCarousel_Button_Widget : public UUserWidget
{
	GENERATED_BODY()

public:

    // Array of possible values
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel")
        TArray<FString> PossibleValues;

    // Current selected index
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel")
        int32 CurrentIndex;

    // Text elements
    UPROPERTY(meta = (BindWidget))
        class UTextBlock* LabelText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* ValueText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* LeftArrowText;

    UPROPERTY(meta = (BindWidget))
        class UTextBlock* RightArrowText;

    // Event when value changes
    UPROPERTY(BlueprintAssignable, Category = "Carousel")
        FOnValueChanged OnValueChanged;

    // Function to cycle values
    UFUNCTION(BlueprintCallable, Category = "Carousel")
        void CycleValue(bool bNext);

    // Get current value
    UFUNCTION(BlueprintPure, Category = "Carousel")
        FString GetCurrentValue() const;

    // Set label text
    UFUNCTION(BlueprintCallable, Category = "Carousel")
        void SetLabel(const FString& NewLabel);

protected:

    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
};
