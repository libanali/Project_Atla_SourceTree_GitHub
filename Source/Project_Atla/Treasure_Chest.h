// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ren_Low_Poly_Character.h"
#include "Components/BoxComponent.h"
#include "Treasure_Chest.generated.h"


UENUM(BlueprintType)
enum class ETreasureChestType : uint8
{
	AttackBoost UMETA(DisplayName = "Attack Boost"),
	DefenseBoost UMETA(DisplayName = "Defense Boost"),
	UnlockTechnique UMETA(DisplayName = "Unlock Technique"),
	RareItem UMETA(DisplayName = "Rare Item")
};




UCLASS()
class PROJECT_ATLA_API ATreasure_Chest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasure_Chest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure Chest")
		ETreasureChestType ChestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure Chest")
		float RewardAmount; // This could be a stat boost amount

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure Chest")
		UAnimationAsset* OpenChestAnimation;

	UFUNCTION(BlueprintCallable)
		void HandleChestReward(ARen_Low_Poly_Character* Ren);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UBoxComponent* BoxCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
