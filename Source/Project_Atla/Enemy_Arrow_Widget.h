// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enemy_Arrow_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UEnemy_Arrow_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
    // Constructor
    UEnemy_Arrow_Widget(const FObjectInitializer& ObjectInitializer);

    // Function to update the rotation of the arrow
    void UpdateArrowRotation(float Angle);

    // Native construct override
    virtual void NativeConstruct() override;

private:
    // The Image widget for the arrow
    UPROPERTY(meta = (BindWidget))
        class UImage* ArrowImage;

    // Function to set the rotation of the arrow
    void SetArrowRotation(float RotationAngle);
	
};
