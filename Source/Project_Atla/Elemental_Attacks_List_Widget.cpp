// Fill out your copyright notice in the Description page of Project Settings.


#include "Elemental_Attacks_List_Widget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Elemental_Attacks_Button_Widget.h"
#include "Test_Button_Widget.h"
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
        //InputMode.SetHideCursorDuringCapture(false);
        PlayerController->SetInputMode(InputMode);
    }
}



void UElemental_Attacks_List_Widget::NativeConstruct()
{

    Super::NativeConstruct();


   // NewPopulateElementalAttackList();
    
    

     // Get the player character
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
       // SetupInputMode();  // Set up input mode first
        PopulateElementalAttackList();  // Then populate the list
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Elemental_Attack_ScrollBox is not valid in SetupWidget"));
    }
}




void UElemental_Attacks_List_Widget::PopulateElementalAttackList()
{
    if (!Elemental_Attack_ScrollBox || !PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Scroll box or PlayerCharacter is null!"));
        return;
    }

    // Clear any existing children
    Elemental_Attack_ScrollBox->ClearChildren();
    TArray<UElemental_Attacks_Button_Widget*> CreatedButtons;

    // Create elemental attack buttons
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
                    // Explicitly set the parent list
                    ElementalButton->SetParentList(this);
                    ElementalButton->SetupButton(ElementalAttack, PlayerCharacter, Index);
                    if (ElementalButton->Elemental_Attack_Button)
                    {
                        ElementalButton->Elemental_Attack_Button->SetIsEnabled(true);
                        ElementalButton->Elemental_Attack_Button->IsFocusable = true;
                    }

                    Elemental_Attack_ScrollBox->AddChild(ElementalButton);
                    CreatedButtons.Add(ElementalButton);
                }
            }
        }
    }

    // Set focus to first button with delay
    if (CreatedButtons.Num() > 0)
    {
        FTimerHandle FocusTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            FocusTimerHandle,
            [FirstButton = CreatedButtons[0]]()
            {
                if (FirstButton && FirstButton->Elemental_Attack_Button)
                {
                    // Just set keyboard focus - let the focus events handle the brush
                    FirstButton->Elemental_Attack_Button->SetKeyboardFocus();
                    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Focus set on first button"));
                }
            },
            0.1f,
                false
                );
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

    // Create three test buttons
    UTest_Button_Widget* FirstButton = CreateWidget<UTest_Button_Widget>(GetWorld(), Test_Button_Widget_Class);
    if (FirstButton)
    {
        Elemental_Attack_ScrollBox->AddChild(FirstButton);

        // Set focus after delay
        FTimerHandle FocusTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            FocusTimerHandle,
            [FirstButton]()
            {
                if (FirstButton && FirstButton->TestButton)
                {
                    FirstButton->TestButton->SetKeyboardFocus();
                    UE_LOG(LogTemp, Log, TEXT("Focus set on first test button"));
                }
            },
            0.001f,  // 0.1 second delay
                false  // Don't loop
                );

        UE_LOG(LogTemp, Log, TEXT("First test button added to scroll box"));
    }

    UTest_Button_Widget* SecondButton = CreateWidget<UTest_Button_Widget>(GetWorld(), Test_Button_Widget_Class);
    if (SecondButton)
    {
        Elemental_Attack_ScrollBox->AddChild(SecondButton);
        UE_LOG(LogTemp, Log, TEXT("Second test button added to scroll box"));
    }

    UTest_Button_Widget* ThirdButton = CreateWidget<UTest_Button_Widget>(GetWorld(), Test_Button_Widget_Class);
    if (ThirdButton)
    {
        Elemental_Attack_ScrollBox->AddChild(ThirdButton);
        UE_LOG(LogTemp, Log, TEXT("Third test button added to scroll box"));
    }
}








void UElemental_Attacks_List_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

    Super::NativeTick(MyGeometry, InDeltaTime);



}


