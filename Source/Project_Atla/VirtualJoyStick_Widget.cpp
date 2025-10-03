// Fill out your copyright notice in the Description page of Project Settings.


#include "VirtualJoyStick_Widget.h"
#include "Engine/Engine.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Texture2D.h"
#include "Slate/SlateBrushAsset.h"
#include "Ren_Low_Poly_Character.h"



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

    if (ThumbImage)
    {
        if (UCanvasPanelSlot* ThumbSlot = Cast<UCanvasPanelSlot>(ThumbImage->Slot))
        {
            // Save the designer-set position as the "true center"
            InitialThumbPosition = ThumbSlot->GetPosition();
        }
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

    // Get the local position relative to the entire widget
    FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InEvent.GetScreenSpacePosition());

    // Get the background position to calculate relative position
    if (BackgroundImage)
    {
        UCanvasPanelSlot* BGSlot = Cast<UCanvasPanelSlot>(BackgroundImage->Slot);
        if (BGSlot)
        {
            FVector2D BackgroundPosition = BGSlot->GetPosition();
            FVector2D BGSize = BGSlot->GetSize();  // Changed from BackgroundSize to BGSize

            // Calculate joystick center
            JoystickCenter = BGSize / 2.0f;

            // Adjust local position to be relative to background
            LocalPosition = LocalPosition - BackgroundPosition;

            // Calculate offset from center
            FVector2D Offset = LocalPosition - JoystickCenter;

            // Clamp to radius
            float Distance = Offset.Size();
            if (Distance > JoystickRadius)
            {
                Offset = Offset.GetSafeNormal() * JoystickRadius;
            }

            // Store thumb position
            CurrentThumbPosition = Offset;

            // Calculate input vector
            if (JoystickRadius > 0)
            {
                InputVector.X = Offset.X / JoystickRadius;
                InputVector.Y = -Offset.Y / JoystickRadius;
            }

            // Update visual position
            UpdateThumbPosition();
        }
    }
}

void UVirtualJoyStick_Widget::ResetJoystick()
{
    bIsPressed = false;
    InputVector = FVector2D::ZeroVector;
    CurrentThumbPosition = FVector2D::ZeroVector;

    if (ThumbImage)
    {
        if (UCanvasPanelSlot* ThumbSlot = Cast<UCanvasPanelSlot>(ThumbImage->Slot))
        {
            // Snap back to the original UMG position
            ThumbSlot->SetPosition(InitialThumbPosition);
        }
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (ARen_Low_Poly_Character* MyChar = Cast<ARen_Low_Poly_Character>(PC->GetPawn()))
        {
            MyChar->MoveForward(0.0f);
            MyChar->MoveRight(0.0f);
        }
    }
}

void UVirtualJoyStick_Widget::UpdateThumbPosition()
{
    if (ThumbImage)
    {
        if (UCanvasPanelSlot* ThumbSlot = Cast<UCanvasPanelSlot>(ThumbImage->Slot))
        {
            FVector2D NewPosition = InitialThumbPosition + CurrentThumbPosition;
            ThumbSlot->SetPosition(NewPosition);
        }
    }
}



void UVirtualJoyStick_Widget::NativeDestruct()
{

    Super::NativeDestruct();

}

void UVirtualJoyStick_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    // Continuously apply input while joystick is pressed
    if (bIsPressed)
    {
        if (APlayerController* PC = GetOwningPlayer())
        {
            if (ARen_Low_Poly_Character* MyChar = Cast<ARen_Low_Poly_Character>(PC->GetPawn()))
            {
                MyChar->MoveForward(InputVector.Y);
                MyChar->MoveRight(InputVector.X);
            }
        }
    }
}
