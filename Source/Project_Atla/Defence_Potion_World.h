// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldItem.h"
#include "Defence_Potion_World.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API ADefence_Potion_World : public AWorldItem
{
	GENERATED_BODY()

public:

	ADefence_Potion_World();


    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
        void OnPickup();


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

};
