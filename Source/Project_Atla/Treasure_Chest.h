// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ren_Low_Poly_Character.h"
#include "Treasure_Chest.generated.h"


UENUM(BlueprintType)
enum class ETreasureChestType : uint8
{
	AttackBoost UMETA(DisplayName = "Attack Boost"),
	DefenseBoost UMETA(DisplayName = "Defense Boost"),
	UnlockTechnique UMETA(DisplayName = "Unlock Technique"),
	RareItem1 UMETA(DisplayName = "Rare Item 1"),
	RareItem2 UMETA(DisplayName = "Rare Item 2")
};



UCLASS()
class PROJECT_ATLA_API ATreasure_Chest : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATreasure_Chest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure Chest")
		ETreasureChestType ChestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* OpenChestAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString TechniqueName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString NotificationText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
		float AttackIncrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
		float DefenceIncrease;

	UFUNCTION(BlueprintCallable)
		void HandleChestReward(ARen_Low_Poly_Character* Ren);

};
