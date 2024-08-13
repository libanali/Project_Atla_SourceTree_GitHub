// Fill out your copyright notice in the Description page of Project Settings.


#include "Strength_Potion_World.h"
#include "Strength_Potion.h"
#include "Ren_Low_Poly_Character.h"

AStrength_Potion_World::AStrength_Potion_World()
{

	ItemClass = UStrength_Potion::StaticClass();

}

void AStrength_Potion_World::Tick(float DeltaTime)
{

    Super::Tick(DeltaTime);

}

void AStrength_Potion_World::OnPickup()
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCharacter && PlayerCharacter->InventoryComponent)
    {
        // Create a new Defense Potion item
        UItem_Class* StrengthPotion = NewObject<UStrength_Potion>(this);
        bool bAdded = PlayerCharacter->InventoryComponent->AddItem(StrengthPotion);

        if (bAdded)
        {
            UE_LOG(LogTemp, Log, TEXT("Defense Potion successfully added to inventory."));
            Destroy();  // Destroy the world item after pickup
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to add Defense Potion to inventory."));
        }
    }


}

void AStrength_Potion_World::BeginPlay()
{

    Super::BeginPlay();

}
