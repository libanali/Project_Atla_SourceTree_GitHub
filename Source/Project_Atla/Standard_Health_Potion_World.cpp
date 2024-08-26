// Fill out your copyright notice in the Description page of Project Settings.


#include "Standard_Health_Potion_World.h"
#include "Standard_Health_Potion.h"
#include "Ren_Low_Poly_Character.h" 


AStandard_Health_Potion_World::AStandard_Health_Potion_World()
{

	ItemClass = UStandard_Health_Potion::StaticClass();

}

void AStandard_Health_Potion_World::OnPickup()
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCharacter && PlayerCharacter->InventoryComponent)
    {
        // Create a new Potion item
        UItem_Class* StandardHealthPotion = NewObject<UStandard_Health_Potion>(this);
        bool bAdded = PlayerCharacter->InventoryComponent->AddItem(StandardHealthPotion);

        if (bAdded)
        {
            UE_LOG(LogTemp, Log, TEXT("Standard Health Potion successfully added to inventory."));
            Destroy();  // Destroy the world item after pickup
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to add Standard Health Potion to inventory."));
        }
    }

}

