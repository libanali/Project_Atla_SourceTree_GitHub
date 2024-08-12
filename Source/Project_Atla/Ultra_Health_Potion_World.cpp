// Fill out your copyright notice in the Description page of Project Settings.


#include "Ultra_Health_Potion_World.h"
#include "Ultra_Health_Potion.h"
#include "Ren_Low_Poly_Character.h" 


AUltra_Health_Potion_World::AUltra_Health_Potion_World()
{

	ItemClass = UUltra_Health_Potion::StaticClass();

}

void AUltra_Health_Potion_World::Tick(float DeltaTime)
{

    Super::Tick(DeltaTime);

}

void AUltra_Health_Potion_World::OnPickup()
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCharacter && PlayerCharacter->InventoryComponent)
    {
        UItem_Class* UltraHealthPotion = NewObject<UUltra_Health_Potion>(this);
        bool bAdded = PlayerCharacter->InventoryComponent->AddItem(UltraHealthPotion);

        if (bAdded)
        {
            UE_LOG(LogTemp, Log, TEXT("Ultra Health Potion successfully added to inventory."));
            Destroy();  // Destroy the world item after pickup
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to add Ultra Health Potion to inventory."));
        }
    }

}

void AUltra_Health_Potion_World::BeginPlay()
{

    Super::BeginPlay();

}
