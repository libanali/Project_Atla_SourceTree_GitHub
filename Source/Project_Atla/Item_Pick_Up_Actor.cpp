// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Pick_Up_Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Ren_Low_Poly_Character.h"
#include "Inventory_Component.h"
#include "Item.h"

// Sets default values
AItem_Pick_Up_Actor::AItem_Pick_Up_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetCollisionProfileName("OverlapAllDynamic");
    RootComponent = CollisionComponent;

    // Initialize the static mesh to represent the item visually
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    // Bind overlap event
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem_Pick_Up_Actor::OnOverlapBegin);


}

// Called when the game starts or when spawned
void AItem_Pick_Up_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem_Pick_Up_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem_Pick_Up_Actor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(OtherActor);
    if (PlayerCharacter && PlayerCharacter->InventoryComponent)
    {
        // Try to add the item to the player's inventory
        if (PlayerCharacter->InventoryComponent->AddItem(Item))
        {
            // Destroy the item actor after pickup
            Destroy();
        }
    }
}

