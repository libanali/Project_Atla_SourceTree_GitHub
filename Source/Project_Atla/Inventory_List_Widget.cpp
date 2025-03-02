// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_List_Widget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Item_Button_Widget.h"
#include "Ren_Low_Poly_Character.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"





void UInventory_List_Widget::NativeConstruct()
{

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


void UInventory_List_Widget::SetupWidget(ARen_Low_Poly_Character* Character)
{

    PlayerCharacter = Character;
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerCharacter passed to SetupWidget"));
        return;
    }

    if (Item_ScrollBox)
    {
        FTimerHandle PopulateTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            PopulateTimerHandle,
            this,
            &UInventory_List_Widget::PopulateItemList,
            0.01f,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Item_ScrollBox is not valid in SetupWidget"));
    }

}




void UInventory_List_Widget::PopulateItemList()
{

    if (!Item_ScrollBox || !PlayerCharacter)
    {
        return;
    }

    UInventory* InventoryComp = PlayerCharacter->FindComponentByClass<UInventory>();
    if (!InventoryComp)
    {
        return;
    }

    // Clear existing buttons
    Item_ScrollBox->ClearChildren();

    // Create sorted array of items
    TArray<FInventoryItem> SortedItems = InventoryComp->Inventory;
    SortedItems.Sort([](const FInventoryItem& A, const FInventoryItem& B) {
        return A.Item.GetDefaultObject()->ItemName < B.Item.GetDefaultObject()->ItemName;
        });

    // Create buttons for each item
    for (const FInventoryItem& InvItem : SortedItems)
    {
        if (ItemButtonClass)
        {
            UItem_Button_Widget* ItemButton = CreateWidget<UItem_Button_Widget>(GetWorld(), ItemButtonClass);
            if (ItemButton)
            {
                ItemButton->SetParentList(this);
                ItemButton->SetupButton(InvItem, PlayerCharacter);
                Item_ScrollBox->AddChild(ItemButton);
            }
        }
    }

    // Set focus to first button with delay
    if (Item_ScrollBox->GetChildrenCount() > 0)
    {
        FTimerHandle FocusTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            FocusTimerHandle,
            [this]()
            {
                if (UItem_Button_Widget* FirstButton = Cast<UItem_Button_Widget>(Item_ScrollBox->GetChildAt(0)))
                {
                    if (FirstButton->Item_Button)
                    {
                        FirstButton->Item_Button->SetKeyboardFocus();
                    }
                }
            },
            0.001f,
                false
                );
    }

}



void UInventory_List_Widget::SetupInputMode()
{


    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(InputMode);
    }



}

