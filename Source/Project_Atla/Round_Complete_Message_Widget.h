// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Round_Complete_Message_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API URound_Complete_Message_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
        // Animation to play when showing the widget
    UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
       class UWidgetAnimation* ShowAnimation;

    // Play the show animation
    UFUNCTION(BlueprintCallable, Category = "Animation")
        void PlayShowAnimation();

    // Play the hide animation
    UFUNCTION(BlueprintCallable, Category = "Animation")
        void PlayHideAnimation();
	
};
