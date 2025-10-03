// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "VirtualJoyStick_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UVirtualJoyStick_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:

    // The input vector that the character will read
    UPROPERTY(BlueprintReadOnly, Category = "Virtual Joystick")
    FVector2D InputVector;

    // Customizable properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtual Joystick")
    float JoystickRadius = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtual Joystick")
    FVector2D BackgroundSize = FVector2D(446.0f, 411.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtual Joystick")
    FVector2D ThumbSize = FVector2D(80.0f, 80.0f);

    // Textures for the joystick visuals
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtual Joystick")
    UTexture2D* BackgroundTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtual Joystick")
    UTexture2D* ThumbTexture;

    // These will be bound to widgets created in the Blueprint designer
    UPROPERTY(meta = (BindWidget))
    class UImage* BackgroundImage;

    UPROPERTY(meta = (BindWidget))
    class UImage* ThumbImage;

    // Optional: Initialize with textures from code
    void InitializeJoystick(UTexture2D* InBackgroundTexture, UTexture2D* InThumbTexture);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Touch input handling
    virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
    virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
    virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

    // Mouse input handling (for PC testing)
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
    bool bIsPressed = false;
    FVector2D CurrentThumbPosition;
    FVector2D JoystickCenter;

    void HandleInput(const FGeometry& InGeometry, const FPointerEvent& InEvent, bool bPressed);
    void ResetJoystick();
    void UpdateThumbPosition();
    FVector2D InitialThumbPosition;

};
