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


    if (!Technique_Button)
    {
        UE_LOG(LogTemp, Warning, TEXT("Technique_Button is nullptr!"));
        return;
    }


    // Clear existing delegates before adding new ones
    Technique_Button->OnClicked.Clear();
    Technique_Button->OnHovered.Clear();
    Technique_Button->OnUnhovered.Clear();

    // Store the original normal brush
    CurrentNormalBrush = Technique_Button->WidgetStyle.Normal;

    // Set up button style for hover/focus states
    FButtonStyle ButtonStyle = Technique_Button->WidgetStyle;
    ButtonStyle.SetHovered(HoveredBrush);
    ButtonStyle.SetPressed(HoveredBrush);

    // Apply the style
    Technique_Button->SetStyle(ButtonStyle);


    // Conditionally set focusability based on technique's unlock status
    if (CurrentTechnique.bIsUnlocked && PlayerCharacter &&
        PlayerCharacter->TechniqueStruct.TechniquePoints >= CurrentTechnique.PointsRequired)
    {
        Technique_Button->SetIsEnabled(true);
        Technique_Button->IsFocusable = true;
        this->SetIsFocusable(true);
    }
    else if (CurrentTechnique.bIsUnlocked && PlayerCharacter &&
        PlayerCharacter->TechniqueStruct.TechniquePoints < CurrentTechnique.PointsRequired)
    {
        Technique_Button->SetIsEnabled(true);
        Technique_Button->IsFocusable = false;
        this->SetIsFocusable(false);
    }
    else
    {
        Technique_Button->SetIsEnabled(false);
        Technique_Button->IsFocusable = false;
        this->SetIsFocusable(false);
    }

    // Bind delegates
    Technique_Button->OnClicked.AddDynamic(this, &UTechnique_Button_Widget::OnTechniqueButtonClicked);
    Technique_Button->OnHovered.AddDynamic(this, &UTechnique_Button_Widget::OnTechniqueButtonHovered);
    Technique_Button->OnUnhovered.AddDynamic(this, &UTechnique_Button_Widget::OnTechniqueButtonUnhovered);

}




void UTechnique_Button_Widget::NativeOnInitialized()
{
    Super::NativeOnInitialized();


}




FReply UTechnique_Button_Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

    if (InKeyEvent.GetKey() == EKeys::Enter ||
        InKeyEvent.GetKey() == EKeys::SpaceBar ||
        InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
    {
        if (Technique_Button)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Key pressed on technique button!"));
            OnTechniqueButtonClicked();
            return FReply::Handled();
        }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

}





FReply UTechnique_Button_Widget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{

    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("NativeOnFocusReceived Called"));
    if (Technique_Button)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Focus Received - Setting Hovered Brush"));
        if (ParentListWidget)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                FString::Printf(TEXT("ParentListWidget valid, has description: %s"),
                    ParentListWidget->DescriptionText ? TEXT("Yes") : TEXT("No")));

            // Add this condition to check if technique is unlocked
            if (ParentListWidget->DescriptionText && CurrentTechnique.bIsUnlocked)
            {
                ParentListWidget->DescriptionText->SetText(FText::FromString(CurrentTechnique.Description));
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,
                    FString::Printf(TEXT("Setting description: %s"), *CurrentTechnique.Description));
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ParentListWidget is null"));
        }

        if (CurrentTechnique.Description.IsEmpty())
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Technique Description is empty"));
        }

        FButtonStyle ButtonStyle = Technique_Button->WidgetStyle;
        ButtonStyle.SetNormal(HoveredBrush);
        Technique_Button->SetStyle(ButtonStyle);
    }

    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

}





void UTechnique_Button_Widget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{

    if (Technique_Button)
    {
        FButtonStyle ButtonStyle = Technique_Button->WidgetStyle;
        ButtonStyle.SetNormal(CurrentNormalBrush);
        Technique_Button->SetStyle(ButtonStyle);

        if (ParentListWidget && ParentListWidget->DescriptionText)
        {
            ParentListWidget->DescriptionText->SetText(FText::GetEmpty());
        }
    }


    Super::NativeOnFocusLost(InFocusEvent);

}





void UTechnique_Button_Widget::SetupButton(FTechnique_Struct TechniqueData, ARen_Low_Poly_Character* Character, int32 Index)
{
    CurrentTechnique = TechniqueData;
    PlayerCharacter = Character;
    TechniqueIndex = Index;

    if (Technique_Name)
    {
        Technique_Name->SetText(FText::FromString(TechniqueData.TechniqueName));
    }

    if (Technique_Points_Required)
    {
        FString PointsText = FString::Printf(TEXT("%d"), TechniqueData.PointsRequired);
        Technique_Points_Required->SetText(FText::FromString(PointsText));
    }

    // Let's verify our parent list is set correctly
    if (ParentListWidget)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
            TEXT("ParentListWidget already set through SetParentList"));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
            TEXT("ParentListWidget not set - trying GetParent()"));
        ParentListWidget = Cast<UTechnique_List_Widget>(GetParent());

        if (!ParentListWidget)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
                TEXT("Could not find ParentListWidget in SetupButton"));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                TEXT("ParentListWidget found successfully through GetParent()"));
        }
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





void UTechnique_Button_Widget::SetButtonTextColor(FLinearColor NewColor)
{

    if (Technique_Button)
    {
        // Set the color of the button text
        Technique_Button->SetColorAndOpacity(NewColor);
    }


}
