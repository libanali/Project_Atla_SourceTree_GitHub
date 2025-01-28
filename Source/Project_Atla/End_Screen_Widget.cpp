// Fill out your copyright notice in the Description page of Project Settings.


#include "End_Screen_Widget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/BackgroundBlur.h"

void UEnd_Screen_Widget::NativeConstruct()
{

	Super::NativeConstruct();



}

void UEnd_Screen_Widget::SetupGameOver(int32 FinalScore, int32 HighScore, int32 RoundNumber)
{
}

void UEnd_Screen_Widget::UpdateStats(float Attack, float Defense, float Elemental, float Health, float NewAttack, float NewDefense, float NewElemental, float NewHealth)
{
}

void UEnd_Screen_Widget::UpdateExpProgress(int32 CurrentLevel, int32 NextLevel, float Progress)
{
}

void UEnd_Screen_Widget::OnRetryClicked()
{
}

void UEnd_Screen_Widget::OnMainMenuClicked()
{
}

void UEnd_Screen_Widget::OnBlurComplete()
{
}

void UEnd_Screen_Widget::OnGameOverTextComplete()
{
}

void UEnd_Screen_Widget::OnResultsPanelComplete()
{
}

void UEnd_Screen_Widget::ShowPage(EGameOverPage Page)
{
}

FText UEnd_Screen_Widget::FormatStatText(const FString& StatName, float CurrentValue, float NewValue)
{
	return FText();
}
