// Fill out your copyright notice in the Description page of Project Settings.


#include "Technique_Button_Widget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"


void UTechnique_Button_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Technique_Button)
    {
        Technique_Button->OnClicked.AddDynamic(this, &UTechnique_Button_Widget::OnTechniqueButtonClicked);
        Technique_Button->OnHovered.AddDynamic(this, &UTechnique_Button_Widget::OnTechniqueButtonHovered);
        Technique_Button->OnUnhovered.AddDynamic(this, &UTechnique_Button_Widget::OnTechniqueButtonUnhovered);

        // Store the normal brush for later use
        CurrentNormalBrush = Technique_Button->WidgetStyle.Normal;
    }

}

void UTechnique_Button_Widget::NativeOnInitialized()
{
    Super::NativeOnInitialized();


}


FReply UTechnique_Button_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}



FReply UTechnique_Button_Widget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    if (Technique_Button && CurrentTechnique.bIsUnlocked)
    {
        OnTechniqueButtonHovered();
    }
    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}


void UTechnique_Button_Widget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{

    if (Technique_Button)
    {
        OnTechniqueButtonUnhovered();
    }
    Super::NativeOnFocusLost(InFocusEvent);

}


void UTechnique_Button_Widget::SetupButton(FTechnique_Struct TechniqueData, ARen_Low_Poly_Character* Character, int32 Index)
{
    CurrentTechnique = TechniqueData;
    PlayerCharacter = Character;
    TechniqueIndex = Index;  // Store the index

    if (Technique_Name)
    {
        Technique_Name->SetText(FText::FromString(TechniqueData.TechniqueName));
    }

    if (Technique_Points_Required)
    {
        FString PointsText = FString::Printf(TEXT("%d"), TechniqueData.PointsRequired);
        Technique_Points_Required->SetText(FText::FromString(PointsText));
    }

}

void UTechnique_Button_Widget::OnTechniqueButtonClicked()
{

    if (PlayerCharacter)
    {
        PlayerCharacter->UseTechnique(TechniqueIndex);
        ReturnToGameplay();
    }

}

void UTechnique_Button_Widget::SetParentList(UTechnique_List_Widget* InParentList)
{

    ParentListWidget = InParentList;


}

void UTechnique_Button_Widget::OnTechniqueButtonHovered()
{

    if (ParentListWidget && ParentListWidget->DescriptionText)
    {
        ParentListWidget->DescriptionText->SetText(FText::FromString(CurrentTechnique.Description));
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Hover Description Updated"));
    }

}

void UTechnique_Button_Widget::OnTechniqueButtonUnhovered()
{

    if (ParentListWidget && ParentListWidget->DescriptionText)
    {
        ParentListWidget->DescriptionText->SetText(FText::GetEmpty());
    }

}
