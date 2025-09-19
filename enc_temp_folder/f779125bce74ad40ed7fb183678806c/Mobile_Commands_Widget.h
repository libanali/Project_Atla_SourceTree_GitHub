// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Mobile_Commands_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UMobile_Commands_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // Constructor
    virtual void NativeConstruct() override;

    // Show/Hide functions
    UFUNCTION(BlueprintCallable, Category = "Commands")
    void ShowCommands();

    UFUNCTION(BlueprintCallable, Category = "Commands")
    void HideCommands();

    UFUNCTION(BlueprintCallable, Category = "Commands")
    void ToggleCommands();

    // Called when any command button is clicked
    UFUNCTION(BlueprintCallable, Category = "Commands")
    void OnCommandExecuted();

    // Button references (set in Blueprint)
    UPROPERTY(meta = (BindWidget))
    class UButton* Attack_Button;

    UPROPERTY(meta = (BindWidget))
    class UButton* Evade_Button;

    UPROPERTY(meta = (BindWidget))
    class UButton* Commands_Button;


private:
    class ARen_Low_Poly_Character* PlayerCharacter;

    bool bIsVisible;
};
