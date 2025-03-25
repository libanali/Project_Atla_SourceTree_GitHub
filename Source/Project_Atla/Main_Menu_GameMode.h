// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Main_Menu_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API AMain_Menu_GameMode : public AGameMode
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> MainMenuWidgetClass;


	UFUNCTION(BlueprintPure, Category = "Gamepad")
	FORCEINLINE bool IsUsingGamepad() const { return bIsUsingGamepad; }

	UFUNCTION(BlueprintCallable, Category = "Gamepad")
	void SetIsUsingGamepad(const bool bIsUsing);

	void DetectGamepadType();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsUsingGamepad;


};
