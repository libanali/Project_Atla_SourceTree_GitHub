// Fill out your copyright notice in the Description page of Project Settings.


#include "Defence_Potion_World.h"
#include "Defence_Potion.h"
#include "Ren_Low_Poly_Character.h" 

ADefence_Potion_World::ADefence_Potion_World()
{

	ItemClass = UDefence_Potion::StaticClass();

}

void ADefence_Potion_World::Tick(float DeltaTime)
{

    Super::Tick(DeltaTime);

}

void ADefence_Potion_World::BeginPlay()
{

    Super::BeginPlay();

}

void ADefence_Potion_World::OnPickup()
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCharacter && PlayerCharacter->InventoryComponent)
    {
        // Create a new Defense Potion item
        UItem_Class* DefencePotion = NewObject<UDefence_Potion>(this);
        bool bAdded = PlayerCharacter->InventoryComponent->AddItem(DefencePotion);

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


