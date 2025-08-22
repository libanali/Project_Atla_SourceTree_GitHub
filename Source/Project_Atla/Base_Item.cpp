// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory.h"

// Sets default values
ABase_Item::ABase_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Create and setup the collision sphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->SetSphereRadius(50.0f);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAll"));

    // Create and setup the static mesh
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(RootComponent);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Default properties
    bIsStackable = false;
    MaxStackSize = 3;
    ItemName = TEXT("Base Item");
}



void ABase_Item::UseItem_Implementation(AActor* User)
{

    UE_LOG(LogTemp, Log, TEXT("%s used by %s"), *ItemName, *User->GetName());



}



void ABase_Item::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    UE_LOG(LogTemp, Log, TEXT("Item %s overlapped with %s"), *ItemName, *OtherActor->GetName());

    // Check if the overlapped actor has an inventory component
    if (UInventory* InventoryComponent = OtherActor->FindComponentByClass<UInventory>())
    {
        // Try to add this item to the inventory
        if (InventoryComponent->AddItem(GetClass()))
        {
            // If successfully added to inventory, destroy the pickup
            Destroy();
        }
    }

}



// Called when the game starts or when spawned
void ABase_Item::BeginPlay()
{
    Super::BeginPlay();

    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABase_Item::OnOverlapBegin);
    SetLifeSpan(10.0f);

}



