// Fill out your copyright notice in the Description page of Project Settings.


#include "Elemental_Attacks_List_Widget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Elemental_Attacks_Button_Widget.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"



void UElemental_Attacks_List_Widget::NativeOnInitialized()
{
    Super::NativeOnInitialized();


}



void UElemental_Attacks_List_Widget::SetupInputMode()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        PlayerController->SetInputMode(InputMode);
    }
}



void UElemental_Attacks_List_Widget::NativeConstruct()
{

    Super::NativeConstruct();


    if (Elemental_Attack_ScrollBox && PlayerCharacter)
    {
      //  PopulateElementalAttackList();
    }

    // Set up input mode after a short delay to ensure widget is fully constructed

/*

    FTimerHandle UnusedHandle;
    GetWorld()->GetTimerManager().SetTimer(
        UnusedHandle,
        FTimerDelegate::CreateUObject(this, &UElemental_Attacks_List_Widget::SetupInputMode),
        0.1f,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Native Construct!"));
    */

    NewPopulateElementalAttackList();
}




void UElemental_Attacks_List_Widget::SetupWidget(ARen_Low_Poly_Character* Character)
{
    PlayerCharacter = Character;

    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerCharacter passed to SetupWidget"));
        return;
    }

    if (Elemental_Attack_ScrollBox)
    {
        PopulateElementalAttackList();
        SetupInputMode();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Elemental_Attack_ScrollBox is not valid in SetupWidget"));
    }
}




void UElemental_Attacks_List_Widget::PopulateElementalAttackList()
{
    if (!PlayerCharacter || !Elemental_Attack_ScrollBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter or Elemental_Attack_ScrollBox is null."));
        return;
    }

    Elemental_Attack_ScrollBox->ClearChildren();
    TArray<UElemental_Attacks_Button_Widget*> CreatedButtons;

    for (const auto& WeaponAttacksPair : PlayerCharacter->WeaponElementalAttacks)
    {
        for (int32 Index = 0; Index < WeaponAttacksPair.Value.ElementalAttacks.Num(); ++Index)
        {
            const FElemental_Struct& ElementalAttack = WeaponAttacksPair.Value.ElementalAttacks[Index];
            if (ElementalAttack.bIsUnlocked && ElementalAttackButtonClass)
            {
                UElemental_Attacks_Button_Widget* ElementalButton = CreateWidget<UElemental_Attacks_Button_Widget>(GetWorld(), ElementalAttackButtonClass);
                if (ElementalButton)
                {
                    ElementalButton->SetupButton(ElementalAttack, PlayerCharacter, Index);
                    Elemental_Attack_ScrollBox->AddChild(ElementalButton);
                    CreatedButtons.Add(ElementalButton);
                }
            }
        }
    }

    // Focus the first button after all buttons are created
    if (CreatedButtons.Num() > 0)
    {
        CreatedButtons[0]->SetKeyboardFocus();

        // Optional: Log success
        if (CreatedButtons[0]->HasKeyboardFocus())
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully set focus to the first button."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to set focus to the first button."));
        }
    }
}




void UElemental_Attacks_List_Widget::NewPopulateElementalAttackList()
{
    if (!Elemental_Attack_ScrollBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("Scroll box is null!"));
        return;
    }

    // Clear any existing children
    Elemental_Attack_ScrollBox->ClearChildren();

    // Create three buttons with different colors
    UButton* FirstButton = NewObject<UButton>(this);
    if (FirstButton)
    {
        FirstButton->SetBackgroundColor(FLinearColor::Blue);
        FirstButton->SetIsEnabled(true);
        FirstButton->IsFocusable = true;
        Elemental_Attack_ScrollBox->AddChild(FirstButton);

        // Set focus after a 0.1 second delay
        FTimerHandle FocusTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            FocusTimerHandle,
            [FirstButton]()
            {
                if (FirstButton)
                {
                    FirstButton->SetKeyboardFocus();
                    UE_LOG(LogTemp, Log, TEXT("Focus set on first button"));
                }
            },
            0.001f,  // 0.1 second delay
                false  // Don't loop
                );

        UE_LOG(LogTemp, Log, TEXT("First button added to scroll box"));
    }

    UButton* SecondButton = NewObject<UButton>(this);
    if (SecondButton)
    {
        SecondButton->SetBackgroundColor(FLinearColor::Red);
        SecondButton->SetIsEnabled(true);
        SecondButton->IsFocusable = true;
        Elemental_Attack_ScrollBox->AddChild(SecondButton);
        UE_LOG(LogTemp, Log, TEXT("Second button added to scroll box"));
    }

    UButton* ThirdButton = NewObject<UButton>(this);
    if (ThirdButton)
    {
        ThirdButton->SetBackgroundColor(FLinearColor::Green);
        ThirdButton->SetIsEnabled(true);
        ThirdButton->IsFocusable = true;
        Elemental_Attack_ScrollBox->AddChild(ThirdButton);
        UE_LOG(LogTemp, Log, TEXT("Third button added to scroll box"));
    }
}




void UElemental_Attacks_List_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);



}


