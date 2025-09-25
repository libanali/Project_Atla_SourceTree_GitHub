// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_List_Widget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Item_Button_Widget.h"
#include "Ren_Low_Poly_Character.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"





void UInventory_List_Widget::NativeConstruct()
{

    Super::NativeConstruct();

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


    // Add this block for mobile scaling
    if (IsRunningOnMobile())
    {
        // Position entire widget in bottom-right
       // SetRenderTransformPivot(FVector2D(1.0f, 1.0f));

        // Scale the containers
        if (Title)  // Your horizontal box
        {
            Title->SetRenderScale(FVector2D(2.0f, 2.0f));
            Title->SetRenderTransformPivot(FVector2D(1.0f, 1.0f));
        }

        if (ItemScrollBoxAndDescription)  // Your vertical box
        {
            ItemScrollBoxAndDescription->SetRenderScale(FVector2D(2.0f, 2.0f));
            ItemScrollBoxAndDescription->SetRenderTransformPivot(FVector2D(1.0f, 1.0f));
        }
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
        // Only set focus if controller is connected
        if (IsControllerConnected() && !IsRunningOnMobile())
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


bool UInventory_List_Widget::IsControllerConnected() const
{
    return FSlateApplication::Get().IsGamepadAttached();
}


bool UInventory_List_Widget::IsRunningOnMobile() const
{
#if PLATFORM_ANDROID || PLATFORM_IOS
    return true;
#else
    // Runtime check for mobile platform
    FString PlatformName = UGameplayStatics::GetPlatformName();
    return (PlatformName == TEXT("Android") || PlatformName == TEXT("IOS"));
#endif
}