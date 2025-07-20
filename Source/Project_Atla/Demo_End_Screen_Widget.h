// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Demo_End_Screen_Widget.generated.h"

/**
 * 
 */

class UButton;
class UWidgetAnimation;
class UBackgroundBlur;
class UTextBlock;
class AResults_camera;

UCLASS()
class PROJECT_ATLA_API UDemo_End_Screen_Widget : public UUserWidget
{
	GENERATED_BODY()
	

public:


	virtual void NativeConstruct() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	 UButton* Main_Menu_Button;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	 UButton* Steam_Store_Button;


};
