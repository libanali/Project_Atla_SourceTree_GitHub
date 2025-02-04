// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enemy_Detection_Arrow.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UEnemy_Detection_Arrow : public UUserWidget
{
	GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    // Updates the arrow’s rotation to point towards the enemy
    UFUNCTION(BlueprintCallable, Category = "Arrow")
        void UpdateArrowRotation(float RotationAngle);

    UPROPERTY(meta = (BindWidget))
       class UImage* ArrowImage; // Assign in the Widget Designer
    
	
};
