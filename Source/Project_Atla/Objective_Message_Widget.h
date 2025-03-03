// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Objective_Message_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UObjective_Message_Widget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ShowAnimation;



    virtual void NativeConstruct() override;

    // Plays the show animation
    UFUNCTION(BlueprintCallable, Category = "Objective")
        void PlayShowAnimation();

    // Plays the hide animation (reverse of show)
    UFUNCTION(BlueprintCallable, Category = "Objective")
        void PlayHideAnimation();


	
};
