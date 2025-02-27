// Fill out your copyright notice in the Description page of Project Settings.


#include "Floating_Combat_Text_Widget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"


void UFloating_Combat_Text_Widget::NativeConstruct()
{

    Super::NativeConstruct();


    bUseScreenSpace = true;
    bIsRemoving = false;
    CurrentLifeTime = 0.0f;
    MaxLifeSpan = 2.0f;
    PositionOffset = FVector2D(0.0f, 0.0f);


    // Start playing fade animation if it exists
    if (FadeOutAnimation)
    {
        PlayAnimation(FadeOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
    }

}

void UFloating_Combat_Text_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);

    // Update lifetime and remove if exceeded
    CurrentLifeTime += InDeltaTime;
    if (CurrentLifeTime >= MaxLifeSpan && !bIsRemoving)
    {
        bIsRemoving = true;
        RemoveFromParent();
        return;
    }

    // Update position if tracking a world object
    if (!TargetWorldPosition.IsZero() && bUseScreenSpace)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            FVector2D ScreenPosition;
            if (PC->ProjectWorldLocationToScreen(TargetWorldPosition, ScreenPosition))
            {
                // Apply float-up effect - moves upward over time
                PositionOffset.Y -= 50.0f * InDeltaTime;

                // Set the position in the viewport
                ScreenPosition += PositionOffset;
                SetPositionInViewport(ScreenPosition);
            }
        }
    }


}

void UFloating_Combat_Text_Widget::Init(const FString& Text, const FLinearColor& Color, bool bIsCritical, float LifeSpan)
{

    if (CombatText)
    {
        // Set text
        CombatText->SetText(FText::FromString(Text));

        // Set color
        CombatText->SetColorAndOpacity(FSlateColor(Color));

        // Make critical hits larger
        if (bIsCritical)
        {
            CombatText->SetRenderScale(FVector2D(1.5f, 1.5f));
        }
    }

    // Set lifespan
    MaxLifeSpan = LifeSpan;


}

void UFloating_Combat_Text_Widget::UpdateWorldPosition(const FVector& NewWorldPos)
{

    TargetWorldPosition = NewWorldPos;

    // Initial positioning
    if (bUseScreenSpace)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            FVector2D ScreenPosition;
            if (PC->ProjectWorldLocationToScreen(TargetWorldPosition, ScreenPosition))
            {
                // Apply initial random offset for variation
                PositionOffset = FVector2D(FMath::RandRange(-20.0f, 20.0f), 0.0f);
                SetPositionInViewport(ScreenPosition + PositionOffset);
            }
        }
    }

}

void UFloating_Combat_Text_Widget::SetTextType(const FString& Type)
{

    if (Type.Equals("Damage", ESearchCase::IgnoreCase))
    {
        // Default white for damage
    }
    else if (Type.Equals("Heal", ESearchCase::IgnoreCase))
    {
        // Set to green for healing
        if (CombatText)
        {
            CombatText->SetColorAndOpacity(FSlateColor(FLinearColor(0.0f, 1.0f, 0.3f)));
        }
    }
    else if (Type.Equals("Freeze", ESearchCase::IgnoreCase))
    {
        // Set to ice blue for freeze effects
        if (CombatText)
        {
            CombatText->SetColorAndOpacity(FSlateColor(FLinearColor(0.0f, 0.7f, 1.0f)));
        }
    }
    else if (Type.Equals("Burn", ESearchCase::IgnoreCase))
    {
        // Set to orange for burn effects
        if (CombatText)
        {
            CombatText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.3f, 0.0f)));
        }
    }



}
