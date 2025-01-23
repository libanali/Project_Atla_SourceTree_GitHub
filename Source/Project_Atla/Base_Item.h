// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Base_Item.generated.h"

UCLASS()
class PROJECT_ATLA_API ABase_Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABase_Item();

    // Core item properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
        FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
        FString Item_Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
        bool bIsStackable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
        int32 MaxStackSize;

    // Components
    UPROPERTY(VisibleAnywhere)
        class USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* ItemMesh;

    // Functions - MODIFIED THIS PART
    UFUNCTION(BlueprintNativeEvent, Category = "Item")
        void UseItem(AActor* User);

    // Remove the virtual keyword here, as BlueprintNativeEvent already implies virtual
    void UseItem_Implementation(AActor* User);



protected:
    virtual void BeginPlay() override;

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

};
