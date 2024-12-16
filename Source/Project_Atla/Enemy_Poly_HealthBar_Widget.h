// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enemy_Poly_HealthBar_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UEnemy_Poly_HealthBar_Widget : public UUserWidget
{
	GENERATED_BODY()

		// Progress bar reference (bind this in the widget blueprint)
		UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;



public:
	// Function to update the health bar
	UFUNCTION(BlueprintCallable, Category = "UI")
		void UpdateHealthBar(float HealthPercentage);


	
};
