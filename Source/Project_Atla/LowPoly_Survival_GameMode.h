// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Enemy_Poly.h"
#include "Enemy_Token_Manager.h"
#include "Score_Struct.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"
#include "Components/BoxComponent.h"  // Include BoxComponent header
#include "Round_Complete_Message_Widget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "LowPoly_Survival_GameMode.generated.h"

/**
 * 
 */




UCLASS()
class PROJECT_ATLA_API ALowPoly_Survival_GameMode : public AGameMode
{
	GENERATED_BODY()


public:
    ALowPoly_Survival_GameMode();

    void UpdateEnemyNumbers();

    UFUNCTION(BlueprintCallable)
        void PlayPowerUpAnim();

    AEnemy_AIController* CurrentAttacker;

    void CycleToNextEnemy();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsSpawningEnemies;

    void ResetAttackCycle();

    void StartEnemyAttackCycle();

    // List of currently spawned enemies
    TArray<AEnemy_Poly*> SpawnedEnemies;

    // List of active enemy controllers
    TArray<AEnemy_AIController*> ActiveEnemies;

    int32 CalculatePointsForEnemy(FScore_Struct EnemyScoreStruct);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rounds")
        int32 SpecialEventInterval = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Zones")
        TArray<AActor*> SpawnZones;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
        bool bHasShownObjectiveMessage;

    UPROPERTY(BlueprintReadWrite, Category = "Spawning")
        bool bStopSpawning;

        
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        USoundBase* LevelMusic;

    UPROPERTY()
        UAudioComponent* LevelMusicComponent;

    UFUNCTION(BlueprintCallable)
        void StopSpawningAndDestroyEnemies();

    UFUNCTION(BlueprintCallable)
        void StopLevelMusic();

    // Current round number
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round")
        int32 CurrentRound;

    void UpdateEnemyBehavior();

    UPROPERTY()
        FTimerHandle EnemyBehaviorUpdateTimer;

    // Returns a spawn location that uses NavMesh and avoids player's line of sight
    FVector GetSmartSpawnLocation();

    // Helper function to find valid NavMesh points near spawn zones
    FVector GetRandomNavMeshPointNearSpawnZone();

    bool bIsGameOver;
    bool bIsShuttingDown;

    void ClearAllTimers();
    void PrepareForLevelTransition();


private:
    FVector GetRandomPointInZone(const FBox& Zone);
    TArray<FTimerHandle> ActiveTimerHandles;


    UPROPERTY()
        FTimerHandle PowerUpStateTimer;

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    // Function to spawn enemies
    void SpawnEnemies();

    // Function to start the next round
    void StartNextRound();

    // Check if all enemies are destroyed
    void CheckForNextRound();

    void CheckIfCanPowerUp();

    // Timer handle to manage spawning rounds
    FTimerHandle SpawnTimerHandle;

    // Delay between each spawn round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float SpawnDelay;

    // Enemy class to spawn
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
        TSubclassOf<AEnemy_Poly> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float RoundDelay;

    // Base number of enemies to spawn each round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        int32 BaseEnemiesPerRound;

    // Number of additional enemies per round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round")
        int32 AdditionalEnemiesPerRound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round")
        int32 NextSpawnRound;

    // Base health for enemies
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float BaseEnemyHealth;

    // Additional health per round
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float AdditionalEnemyHealthPerRound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawning")
        float BaseSpawnDelay;  // Initial delay between spawns in the first round

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawning")
        float MinSpawnDelay;   // Minimum delay between spawns

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawning")
        float DelayDecreasePerRound;  // How much the delay decreases each round

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        float SpawnRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        TSubclassOf<AEnemy_Poly> BP_Spider;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        TSubclassOf<AEnemy_Poly> BP_Wolf;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        TSubclassOf<AEnemy_Poly> BP_RockTroll;

    TSubclassOf<AEnemy_Poly> GetEnemyClassForCurrentRound();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
        FScore_Struct SpiderScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
        FScore_Struct WolfScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
        FScore_Struct RockTrollScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
        bool bIsPowerUpSpawned;

    // Widget class for objective messages
    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<class UObjective_Message_Widget> ObjectiveMessageWidgetClass;

    UPROPERTY()
        class UObjective_Message_Widget* ObjectiveMessageWidget;

    // Text to display as the objective
    UPROPERTY(EditDefaultsOnly, Category = "UI")
        FText ObjectiveText;

    // Timer handles for the sequence
    FTimerHandle ObjectiveInitialDelayTimer;
    FTimerHandle ObjectiveDisplayTimer;

    // Functions for sequence control
    void ShowObjectiveMessage();
    void HideObjectiveMessage();
    void StartGameAfterObjective();


    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<URound_Complete_Message_Widget> RoundCompleteWidgetClass;

    UPROPERTY()
        URound_Complete_Message_Widget* RoundCompleteWidget;

    void ShowRoundCompleteMessage();
    void HideRoundCompleteMessage();

    FTimerHandle RoundCompleteDisplayTimer;



    UFUNCTION()
        void OnEnemyDestroyed();

    UFUNCTION(BlueprintCallable)
        void ActivateRandomPowerUp();

    
    UFUNCTION(BlueprintCallable)
        void ReturnCamera();

    FTimerHandle PowerUpAnimTimer;
    FTimerHandle ReturnCameraTimer;
    FTimerHandle CheckPowerUp;

    FVector GetRandomPointNearPlayer();

    FDelegateHandle OnOverlayActivatedDelegateHandle;
    void OnSteamOverlayActivated(bool bIsActive);
    void SetupSteamOverlayDetection();




private:

    // Function to assign numbers to enemies
    void AssignEnemyNumbers();

    // Timer for updating enemy numbers
    FTimerHandle NumberUpdateTimer;

    FTimerHandle PowerUpCheckTimer;

	
};
