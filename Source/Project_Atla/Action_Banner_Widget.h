// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Action_Banner_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UAction_Banner_Widget : public UUserWidget
{
	GENERATED_BODY()


public:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* ActionText;

    UFUNCTION(BlueprintCallable)
        void SetText(const FString& Text);

    virtual void NativeConstruct() override;

private:

    FTimerHandle RemoveTimer;

    UFUNCTION()
        void RemoveFromParent_Delayed();
	
};
