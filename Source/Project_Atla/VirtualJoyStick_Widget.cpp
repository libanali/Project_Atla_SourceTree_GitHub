// Fill out your copyright notice in the Description page of Project Settings.


#include "VirtualJoyStick_Widget.h"
#include "Engine/Engine.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Texture2D.h"
#include "Slate/SlateBrushAsset.h"



void UVirtualJoyStick_Widget::NativeConstruct()
{

    Super::NativeConstruct();

    // Initialize values
    CurrentThumbPosition = FVector2D::ZeroVector;
    InputVector = FVector2D::ZeroVector;

    // Set textures if they're assigned
    if (BackgroundImage && BackgroundTexture)
    {
        BackgroundImage->SetBrushFromTexture(BackgroundTexture);
    }

    if (ThumbImage && ThumbTexture)
    {
        ThumbImage->SetBrushFromTexture(ThumbTexture);
    }

    // Make sure widget is visible and can receive input
    SetVisibility(ESlateVisibility::Visible);

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Virtual Joystick Widget Created"));
}



void UVirtualJoyStick_Widget::InitializeJoystick(UTexture2D* InBackgroundTexture, UTexture2D* InThumbTexture)
{
    BackgroundTexture = InBackgroundTexture;
    ThumbTexture = InThumbTexture;

    // Update the images if they exist
    if (BackgroundImage && BackgroundTexture)
    {
        BackgroundImage->SetBrushFromTexture(BackgroundTexture);
    }

    if (ThumbImage && ThumbTexture)
    {
        ThumbImage->SetBrushFromTexture(ThumbTexture);
    }
}

FReply UVirtualJoyStick_Widget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
    HandleInput(InGeometry, InGestureEvent, true);
    return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
}

FReply UVirtualJoyStick_Widget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
    if (bIsPressed)
    {
        HandleInput(InGeometry, InGestureEvent, true);
    }
    return FReply::Handled();
}

FReply UVirtualJoyStick_Widget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
    ResetJoystick();
    return FReply::Handled().ReleaseMouseCapture();
}

FReply UVirtualJoyStick_Widget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        HandleInput(InGeometry, InMouseEvent, true);
        return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
    }
    return FReply::Unhandled();
}

FReply UVirtualJoyStick_Widget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsPressed && InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        HandleInput(InGeometry, InMouseEvent, true);
        return FReply::Handled();
    }
    return FReply::Unhandled();
}


FReply UVirtualJoyStick_Widget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        ResetJoystick();
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}


void UVirtualJoyStick_Widget::HandleInput(const FGeometry& InGeometry, const FPointerEvent& InEvent, bool bPressed)
{
    bIsPressed = bPressed;

    if (!bPressed)
    {
        ResetJoystick();
        return;
    }

    // Get the local position where the user clicked/touched
    FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InEvent.GetScreenSpacePosition());

    // Calculate the center of our joystick (center of the background image)
    JoystickCenter = BackgroundSize / 2.0f;

    // Calculate offset from center
    FVector2D Offset = LocalPosition - JoystickCenter;

    // Clamp the offset to the joystick radius
    float Distance = Offset.Size();
    if (Distance > JoystickRadius)
    {
        Offset = Offset.GetSafeNormal() * JoystickRadius;
    }

    // Store the thumb position for visual update
    CurrentThumbPosition = Offset;

    // Calculate input vector (normalized to -1 to 1 range)
    InputVector.X = Offset.X / JoystickRadius;
    InputVector.Y = -Offset.Y / JoystickRadius; // Invert Y for typical game controls

    // Update the visual position of the thumb
    UpdateThumbPosition();

}

void UVirtualJoyStick_Widget::ResetJoystick()
{
    bIsPressed = false;
    InputVector = FVector2D::ZeroVector;
    CurrentThumbPosition = FVector2D::ZeroVector;

    // Reset thumb to center
    UpdateThumbPosition();
}

void UVirtualJoyStick_Widget::UpdateThumbPosition()
{
    if (ThumbImage)
    {
        UCanvasPanelSlot* ThumbSlot = Cast<UCanvasPanelSlot>(ThumbImage->Slot);
        if (ThumbSlot)
        {
            // Calculate new position (center + offset - half thumb size)
            FVector2D NewPosition = JoystickCenter + CurrentThumbPosition - (ThumbSize / 2.0f);
            ThumbSlot->SetPosition(NewPosition);
        }
    }
}



void UVirtualJoyStick_Widget::NativeDestruct()
{

    Super::NativeDestruct();

}
