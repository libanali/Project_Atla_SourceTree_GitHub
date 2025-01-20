// Fill out your copyright notice in the Description page of Project Settings.


#include "Technique_List_Widget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Technique_Button_Widget.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"



void UTechnique_List_Widget::NativeConstruct()
{

    Super::NativeConstruct();


    ARen_Low_Poly_Character* Character = Cast<ARen_Low_Poly_Character>(GetOwningPlayerPawn());

    if (Character)
    {
        SetupWidget(Character);
        UE_LOG(LogTemp, Log, TEXT("SetupWidget called from NativeConstruct"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get player character in NativeConstruct"));
    }


}



void UTechnique_List_Widget::NativeOnInitialized()
{






}




void UTechnique_List_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{



}



void UTechnique_List_Widget::SetupWidget(ARen_Low_Poly_Character* Character)
{

    PlayerCharacter = Character;
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerCharacter passed to SetupWidget"));
        return;
    }

    if (Technique_ScrollBox)
    {
        bool bIsLoadedGame = PlayerCharacter->bIsGameLoaded;

        if (bIsLoadedGame)
        {
            FTimerHandle PopulateTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                PopulateTimerHandle,
                this,
                &UTechnique_List_Widget::PopulateTechniqueList,
                0.5f,
                false
            );
        }
        else
        {
            PopulateTechniqueList();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Technique_ScrollBox is not valid in SetupWidget"));
    }


}



void UTechnique_List_Widget::PopulateTechniqueList()
{

    if (!Technique_ScrollBox || !PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Scroll box or PlayerCharacter is null!"));
        return;
    }

    // Clear existing buttons
    Technique_ScrollBox->ClearChildren();

    // Sort techniques by required level
    TArray<FTechnique_Struct> SortedTechniques = PlayerCharacter->Techniques;
    SortedTechniques.Sort([](const FTechnique_Struct& A, const FTechnique_Struct& B) {
        return A.RequiredLevel < B.RequiredLevel;
        });

    // Create buttons for all techniques
    for (int32 Index = 0; Index < SortedTechniques.Num(); Index++)
    {
        const FTechnique_Struct& Technique = SortedTechniques[Index];

        if (TechniqueButtonClass)
        {
            UTechnique_Button_Widget* TechniqueButton = CreateWidget<UTechnique_Button_Widget>(
                GetWorld(), TechniqueButtonClass);

            if (TechniqueButton)
            {
                TechniqueButton->SetParentList(this);

                // Find the original index in the unsorted array
                int32 OriginalIndex = PlayerCharacter->Techniques.Find(Technique);
                TechniqueButton->SetupButton(Technique, PlayerCharacter, OriginalIndex);

                // Configure button based on unlock status
                if (TechniqueButton->Technique_Button)
                {
                    bool bShouldEnable = Technique.bIsUnlocked &&
                        PlayerCharacter->TechniqueStruct.TechniquePoints >= Technique.PointsRequired;

                    TechniqueButton->Technique_Button->SetIsEnabled(bShouldEnable);
                    TechniqueButton->Technique_Button->IsFocusable = bShouldEnable;
                    TechniqueButton->SetRenderOpacity(bShouldEnable ? 1.0f : 0.5f);
                }

                Technique_ScrollBox->AddChild(TechniqueButton);

                UE_LOG(LogTemp, Warning, TEXT("Added Technique: %s (Level %d, Unlocked: %s, Points Required: %d)"),
                    *Technique.TechniqueName,
                    Technique.RequiredLevel,
                    Technique.bIsUnlocked ? TEXT("TRUE") : TEXT("FALSE"),
                    Technique.PointsRequired);
            }
        }
    }

    // Set focus to first button with delay
    if (Technique_ScrollBox->GetChildrenCount() > 0)
    {
        FTimerHandle FocusTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            FocusTimerHandle,
            [this]()
            {
                if (Technique_ScrollBox->GetChildAt(0))
                {
                    UTechnique_Button_Widget* FirstButton =
                        Cast<UTechnique_Button_Widget>(Technique_ScrollBox->GetChildAt(0));

                    if (FirstButton && FirstButton->Technique_Button)
                    {
                        FirstButton->Technique_Button->SetKeyboardFocus();
                    }
                }
            },
            0.1f,
                false
                );
    }

}




void UTechnique_List_Widget::SetupInputMode()
{

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        //InputMode.SetHideCursorDuringCapture(false);
        PlayerController->SetInputMode(InputMode);
    }
  

}
