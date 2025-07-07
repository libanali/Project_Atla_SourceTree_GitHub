// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Demo_End_Screen_Widget.generated.h"

/**
 * 
 */



UCLASS()
class PROJECT_ATLA_API UDemo_End_Screen_Widget : public UUserWidget
{
	GENERATED_BODY()
	

public:


	virtual void NativeConstruct() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Main_Menu_Button;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Steam_Store_Button;
};
