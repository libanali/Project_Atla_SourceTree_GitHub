// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Floating_Combat_Text_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UFloating_Combat_Text_Widget : public UUserWidget
{
	GENERATED_BODY()


public:
    //UFloating_Combat_Text_Widget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Initialize the widget with text, color and other properties
    void Init(const FString& Text, const FLinearColor& Color, bool bIsCritical = false, float LifeSpan = 2.0f);

    // Update the world position for the widget to follow
    void UpdateWorldPosition(const FVector& NewWorldPos);

    // Set the type of floating text (damage, effect, etc.)
    void SetTextType(const FString& Type);

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UTextBlock* CombatText;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
        UWidgetAnimation* FadeOutAnimation;

    // World position to track
    FVector TargetWorldPosition;

    // Current age of the widget
    float CurrentLifeTime;

    // Maximum time the widget should live
    float MaxLifeSpan;

    // Offset from the target position
    FVector2D PositionOffset;

    // Use screenspace or widget space
    bool bUseScreenSpace;

    // Is the widget being removed
    bool bIsRemoving;
	
};
