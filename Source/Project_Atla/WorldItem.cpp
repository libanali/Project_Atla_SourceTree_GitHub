// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldItem.h"
#include "Ren_Low_Poly_Character.h"


// Sets default values
AWorldItem::AWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;

}

// Called when the game starts or when spawned
void AWorldItem::BeginPlay()
{
	Super::BeginPlay();

    if (ItemClass)
    {
        ItemInstance = NewObject<UItem_Class>(this, ItemClass);
    }
	
}

// Called every frame
void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldItem::InitializeItem(UItem_Class* NewItem)
{

    if (NewItem)
    {
        Item = NewItem;
        ItemName = Item->ItemName;
    }


}

void AWorldItem::UseItem(ARen_Low_Poly_Character* Ren)
{
    if (ItemInstance)

    {

        ItemInstance->UseItem(Ren);

    }


}

