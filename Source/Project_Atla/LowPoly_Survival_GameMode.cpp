// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_Survival_GameMode.h"
#include "Enemy.h"
#include "Ren_Low_Poly_Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Enemy_Token_Manager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Objective_Message_Widget.h"
#include "Components/AudioComponent.h"
#include "Game_Instance.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "GenericPlatform/IInputInterface.h"
#include "InputCoreTypes.h"
#include "Steam/steam_api.h"
#include "Steam/isteaminput.h"




ALowPoly_Survival_GameMode::ALowPoly_Survival_GameMode()
{

    RoundDelay = 2.5f;
    BaseEnemiesPerRound = 3;
    SpawnRadius = 1200.0f;
    CurrentRound = 1;
    AdditionalEnemyHealthPerRound = 30.0f;
    AdditionalEnemiesPerRound = 1.9f;
    BaseSpawnDelay = 2.0f;         // Initial delay between spawns in the first round
    MinSpawnDelay = 1.5f;          // Minimum delay between spawns in later rounds
    DelayDecreasePerRound = 0.1f;
    bIsSpawningEnemies = false;
    bIsPowerUpSpawned = false;
    bStopSpawning = false;
    bIsShuttingDown = false;
    bHasShownObjectiveMessage = false;




}



void ALowPoly_Survival_GameMode::UpdateEnemyNumbers()
{
    if (SpawnedEnemies.Num() == 0) return;

    // If no enemy is currently attacking, assign one
    if (CurrentAttacker == nullptr || !CurrentAttacker->IsValidLowLevel())
    {
        // Get the first enemy in the list as the new attacker
        AEnemy_AIController* NewAttacker = Cast<AEnemy_AIController>(SpawnedEnemies[0]->GetController());
        if (NewAttacker)
        {
            CurrentAttacker = NewAttacker;
            CurrentAttacker->AttackPlayer();
        }
    }

    // Loop through all enemies
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        AEnemy_AIController* EnemyController = Cast<AEnemy_AIController>(Enemy->GetController());
        if (EnemyController)
        {
            if (EnemyController != CurrentAttacker)
            {
                // Strafe if this enemy is not the current attacker
                EnemyController->StrafeAroundPlayer();
            }
        }
    }
}


void ALowPoly_Survival_GameMode::StopSpawningAndDestroyEnemies()
{

    // Set the flag to stop further spawning
    bStopSpawning = true;
    bIsGameOver = true;  // Add this line
    StopLevelMusic();

    // Destroy all spawned enemies - with safety checks
    for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; i--)
    {
        if (SpawnedEnemies.IsValidIndex(i) && SpawnedEnemies[i] && !SpawnedEnemies[i]->IsPendingKillPending())
        {
            SpawnedEnemies[i]->Destroy();
        }
    }

    SpawnedEnemies.Empty();
}


void ALowPoly_Survival_GameMode::StopLevelMusic()
{
    if (LevelMusicComponent && LevelMusicComponent->IsPlaying())
    {
        // 1.5 second fade to silence using the default linear curve
        LevelMusicComponent->FadeOut(1.5f, 0.0f);

        // Set a timer to stop the component after the fade completes
        FTimerHandle StopMusicTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            StopMusicTimerHandle,
            [this]()
            {
                if (LevelMusicComponent && LevelMusicComponent->IsValidLowLevel())
                {
                    LevelMusicComponent->Stop();
                }
            },
            1.6f, // Slightly longer than the fade time to ensure fade completes
            false
        );
    }
}

void ALowPoly_Survival_GameMode::UpdateEnemyBehavior()
{

    if (bIsShuttingDown)
    {
        // Return a safe default during shutdown
        return;
    }

    // Update behavior parameters for all enemies
   // This replaces the old UpdateEnemyNumbers method

   // Exit if no enemies
    if (SpawnedEnemies.Num() == 0) return;

    // Tell each enemy the current total count (for spacing)
    int32 TotalActive = 0;
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (Enemy && !Enemy->bIsDead)
        {
            TotalActive++;
        }
    }

    // Scale aggression based on remaining enemies (fewer enemies = more aggressive)
    float AggressionScale = 1.0f;
    if (TotalActive > 0)
    {
        // More enemies = slightly less individual aggression
        AggressionScale = FMath::Max(0.8f, 1.0f - ((TotalActive - 1) * 0.05f));

        // Scale with round number for increased difficulty
        float RoundScale = 1.0f + (CurrentRound * 0.05f);
        AggressionScale *= RoundScale;
    }

    // Update behavior parameters for each enemy
    int32 ActiveIndex = 0;
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (!Enemy || Enemy->bIsDead) continue;

        AEnemy_AIController* EnemyController = Cast<AEnemy_AIController>(Enemy->GetController());
        if (EnemyController)
        {
            // Update total enemy count
            EnemyController->SetTotalEnemyCount(TotalActive);

            // Set enemy number (for spacing)
            EnemyController->SetEnemyNumber(ActiveIndex);

            // Update aggression with slight variation between enemies
            float IndividualAggression = AggressionScale * FMath::RandRange(0.9f, 1.1f);
            EnemyController->SetAggressionFactor(IndividualAggression);

            ActiveIndex++;
        }
    }

}

FVector ALowPoly_Survival_GameMode::GetSmartSpawnLocation()
{
    // CRITICAL SAFETY CHECK: Check for multiple failure conditions
    if (bIsShuttingDown || !IsValid(this) || !GetWorld() || GetWorld()->bIsTearingDown)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetSmartSpawnLocation: Early exit during shutdown"));
        return FVector::ZeroVector;
    }

    // Wrap all player character access in safety checks
    ARen_Low_Poly_Character* Player = nullptr;
    if (IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
    {
        Player = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    }

    if (!Player)
    {
        // Another safety check - validate spawn zones array
        if (SpawnZones.Num() == 0 || !IsValid(SpawnZones[0]))
        {
            return FVector::ZeroVector;
        }
        return GetRandomNavMeshPointNearSpawnZone();
    }

    // Try multiple locations to find one not in player's view
    const int32 MaxAttempts = 5;
    for (int32 i = 0; i < MaxAttempts; i++)
    {
        FVector CandidateLocation = GetRandomNavMeshPointNearSpawnZone();

        // IMPORTANT: Perform ground check to ensure valid surface
        FVector StartTrace = CandidateLocation + FVector(0, 0, 50);  // Start 50 units above
        FVector EndTrace = CandidateLocation - FVector(0, 0, 100);   // Trace 100 units down

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.bTraceComplex = true;

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartTrace,
            EndTrace,
            ECC_Visibility,  // or use your own trace channel
            QueryParams
        );

        // Only use locations where we hit solid ground
        if (bHit)
        {
            // Check if location is in player's view
            FVector AdjustedLocation = HitResult.Location + FVector(0, 0, 5); // 5 units above ground
            FVector PlayerToSpawn = AdjustedLocation - Player->GetActorLocation();
            float DistanceToPlayer = PlayerToSpawn.Size();

            PlayerToSpawn.Normalize();
            FVector PlayerForward = Player->GetActorForwardVector();
            float DotProduct = FVector::DotProduct(PlayerForward, PlayerToSpawn);

            // If behind player or far enough away, use this location
            if (DotProduct < 0.0f || DistanceToPlayer > 1500.0f)
            {
                return AdjustedLocation;
            }
        }
    }

    // If we couldn't find a suitable location, try a simpler approach
    // Do a ground trace directly from a spawn zone
    AActor* SpawnZone = SpawnZones[FMath::RandRange(0, SpawnZones.Num() - 1)];
    if (SpawnZone)
    {
        FVector StartTrace = SpawnZone->GetActorLocation() + FVector(0, 0, 100);
        FVector EndTrace = StartTrace - FVector(0, 0, 200);

        FHitResult HitResult;
        GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility);

        if (HitResult.bBlockingHit)
        {
            return HitResult.Location + FVector(0, 0, 5);
        }
    }

    // Last resort: return player location with offset
    if (Player)
    {
        return Player->GetActorLocation() + FVector(0, 0, 10) + (FMath::VRand() * 1000);
    }

    return FVector::ZeroVector;
}

FVector ALowPoly_Survival_GameMode::GetRandomNavMeshPointNearSpawnZone()
{
    // Check if we have any spawn zones
    if (SpawnZones.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No spawn zones found!"));
        return FVector::ZeroVector;
    }

    // Pick a random spawn zone
    AActor* RandomSpawnZone = SpawnZones[FMath::RandRange(0, SpawnZones.Num() - 1)];
    FVector SpawnZoneLocation = RandomSpawnZone->GetActorLocation();

    // Get the navigation system
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Navigation system not found!"));
        return SpawnZoneLocation;
    }

    // Find a random point on the nav mesh near the spawn zone
    FNavLocation ResultLocation;
    bool bFoundPoint = NavSystem->GetRandomReachablePointInRadius(
        SpawnZoneLocation,
        500.0f, // Adjust radius as needed
        ResultLocation);

    if (bFoundPoint)
    {
        return ResultLocation.Location;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find valid nav mesh location!"));
        return SpawnZoneLocation;
    }
}

void ALowPoly_Survival_GameMode::ClearAllTimers()
{

    GetWorldTimerManager().ClearAllTimersForObject(this);

}


void ALowPoly_Survival_GameMode::PrepareForLevelTransition()
{

    // Set the shutdown flag first so all functions know we're shutting down
    bIsShuttingDown = true;
    bStopSpawning = true;  // Make extra sure spawning stops

    UE_LOG(LogTemp, Warning, TEXT("EMERGENCY SHUTDOWN: Level transition preparation starting"));

    // Immediately pause the game to freeze all game logic
    UGameplayStatics::SetGamePaused(GetWorld(), true);

    // Release any player controller references immediately
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->DisableInput(PC);
    }

    // Stop all timers for this object
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    }

    // Disable all ticking for game mode
    SetActorTickEnabled(false);
    PrimaryActorTick.bCanEverTick = false;

    // Nullify and clear enemy references to prevent access
    for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; i--)
    {
        if (SpawnedEnemies.IsValidIndex(i))
        {
            if (SpawnedEnemies[i])
            {
                // Disable physics and collision
                UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(SpawnedEnemies[i]->GetRootComponent());
                if (RootComp)
                {
                    RootComp->SetSimulatePhysics(false);
                    RootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                }

                // Disable AI - use your existing DisableAI method
                if (AEnemy_AIController* EnemyAI = Cast<AEnemy_AIController>(SpawnedEnemies[i]->GetController()))
                {
                    // Call your custom DisableAI method that's already implemented
                    EnemyAI->DisableAI();
                    EnemyAI->StopMovement();

                    // Unpossess is still useful as an additional safety measure
                    EnemyAI->UnPossess();
                }

                // Disable tick
                SpawnedEnemies[i]->SetActorTickEnabled(false);

                // Make invisible
                SpawnedEnemies[i]->SetActorHiddenInGame(true);

                // Finally destroy
                SpawnedEnemies[i]->Destroy();
            }
            SpawnedEnemies[i] = nullptr;
        }
    }
    SpawnedEnemies.Empty();

    // Force garbage collection
    GEngine->ForceGarbageCollection(true);

    UE_LOG(LogTemp, Warning, TEXT("EMERGENCY SHUTDOWN: Game mode prepared for level transition"));
}



void ALowPoly_Survival_GameMode::BeginPlay()
{

    Super::BeginPlay();



    SetupSteamOverlayDetection();



    if (FSlateApplication::Get().GetPlatformApplication()->IsGamepadAttached())
    {
        GEngine->AddOnScreenDebugMessage(2, 2.5f, FColor::Green, TEXT("Gamepad Connected!"));

        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            PlayerController->bShowMouseCursor = false;
        }

    }
    else
    {
        GEngine->AddOnScreenDebugMessage(2, 2.5f, FColor::Black, TEXT("No Gamepad Connected!"));

        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            PlayerController->bShowMouseCursor = true;
        }
    }




    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    FInputModeUIOnly UIOnly;
    PlayerController->SetInputMode(UIOnly);



        // 1. Find all spawn zones first
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawn Zone"), FoundActors);

        SpawnZones.Empty();
        for (AActor* Actor : FoundActors)
        {
            if (Actor)
            {
                SpawnZones.Add(Actor);
            }
        }

      

        SpecialEventInterval = FMath::RandRange(1, 3);

        GetWorld()->GetTimerManager().SetTimer(
            NumberUpdateTimer,
            this,
            &ALowPoly_Survival_GameMode::UpdateEnemyNumbers,
            5.0f,
            true
        );

        GetWorld()->GetTimerManager().SetTimer(
            EnemyBehaviorUpdateTimer,
            this,
            &ALowPoly_Survival_GameMode::UpdateEnemyBehavior,
            5.0f,
            true
        );


        GetWorld()->GetTimerManager().SetTimer(
            ObjectiveInitialDelayTimer,
            this,
            &ALowPoly_Survival_GameMode::ShowObjectiveMessage,
            1.5f,
            false
        );

}



void ALowPoly_Survival_GameMode::Tick(float DeltaTime)
{




    OnEnemyDestroyed();



}



void ALowPoly_Survival_GameMode::SpawnEnemies()
{  

    // Early return if shutting down or spawning is disabled
    if (bIsShuttingDown || bStopSpawning || !GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawning aborted - shutting down or spawning disabled"));
        return;
    }


    // Early return if EnemyClass is not set
    if (EnemyClass == nullptr) return;

    UWorld* World = GetWorld();
    if (World == nullptr) return;

    // Mark that spawning is starting
    bIsSpawningEnemies = true;

    ARen_Low_Poly_Character* Ren = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (Ren)
    {
        Ren->HealthStruct.CurrentHealth = Ren->HealthStruct.MaxHealth;
        Ren->ManaStruct.CurrentMana = Ren->ManaStruct.MaxMana;
    }

    // Determine the number of enemies to spawn for the current round
    int32 EnemiesToSpawn = 4 + (CurrentRound - 1) * AdditionalEnemiesPerRound;
    float LocalSpawnDelay = FMath::Max(MinSpawnDelay, BaseSpawnDelay - (CurrentRound - 1) * DelayDecreasePerRound);


    for (int32 i = 0; i < EnemiesToSpawn; i++)
    {
        // Create a weak pointer to this - IMPORTANT CHANGE
        TWeakObjectPtr<ALowPoly_Survival_GameMode> WeakThis = this;

        FTimerHandle LocalSpawnTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(LocalSpawnTimerHandle, [WeakThis, i, EnemiesToSpawn]()
            {
                // IMPORTANT: Early safety check - if this object is invalid or shutting down, exit immediately
                if (!WeakThis.IsValid() || WeakThis->bIsShuttingDown || WeakThis->bStopSpawning)
                {
                    return;
                }

                // Get a safe pointer to use inside the lambda
                ALowPoly_Survival_GameMode* SafeThis = WeakThis.Get();

                // Use smart spawn location instead of random point
                FVector SpawnLocation = SafeThis->GetSmartSpawnLocation();
                FRotator SpawnRotation = FRotator::ZeroRotator;

                TSubclassOf<AEnemy_Poly> EnemyToSpawnClass = SafeThis->GetEnemyClassForCurrentRound();

                if (EnemyToSpawnClass)
                {
                    // Use improved spawn parameters
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                    AEnemy_Poly* SpawnedEnemy = SafeThis->GetWorld()->SpawnActor<AEnemy_Poly>(EnemyToSpawnClass, SpawnLocation, SpawnRotation, SpawnParams);
                    if (SpawnedEnemy)
                    {
                        UE_LOG(LogTemp, Log, TEXT("Spawned enemy at location: %s"), *SpawnLocation.ToString());

                        // Ensure proper physics and collision setup
                        UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(SpawnedEnemy->GetRootComponent());
                        if (RootComp)
                        {
                            RootComp->SetSimulatePhysics(false);
                            RootComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                        }

                        // Ensure the character movement component is properly initialized
                        if (SpawnedEnemy->GetCharacterMovement())
                        {
                            // IMPORTANT: Use weak pointer for the inner lambda as well
                            TWeakObjectPtr<AEnemy_Poly> WeakEnemy = SpawnedEnemy;
                            FTimerHandle MovementTimerHandle;
                            SafeThis->GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, [WeakEnemy]()
                                {
                                    if (!WeakEnemy.IsValid() || WeakEnemy->IsPendingKillEnabled())
                                    {
                                        return;
                                    }
                                    WeakEnemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
                                }, 0.2f, false);

                            // Track this timer handle
                            SafeThis->ActiveTimerHandles.Add(MovementTimerHandle);
                        }

                        // Increase enemy health based on the current round
                        float AddedEnemyHealth = SpawnedEnemy->MaxEnemyHealth + (SafeThis->CurrentRound - 1) * SafeThis->AdditionalEnemyHealthPerRound;
                        SpawnedEnemy->IncreaseEnemyHealth(AddedEnemyHealth, true);

                        // Add the spawned enemy to the list
                        SafeThis->SpawnedEnemies.Add(SpawnedEnemy);

                        // Initialize the AI controller with the appropriate enemy type
                        AEnemy_AIController* AIController = Cast<AEnemy_AIController>(SpawnedEnemy->GetController());
                        if (AIController)
                        {
                            // Set enemy type based on class
                            EEnemyType EnemyType;
                            if (EnemyToSpawnClass == SafeThis->BP_Spider)
                            {
                                EnemyType = EEnemyType::Spider;
                            }
                            else if (EnemyToSpawnClass == SafeThis->BP_Wolf)
                            {
                                EnemyType = EEnemyType::Wolf;
                            }
                            else if (EnemyToSpawnClass == SafeThis->BP_RockTroll)
                            {
                                EnemyType = EEnemyType::RockTroll;
                            }
                            else
                            {
                                EnemyType = EEnemyType::Spider; // Default
                            }

                            // Initialize AI behavior for this enemy type
                            AIController->InitializeForEnemyType(EnemyType);

                            // Add some randomization to make each enemy slightly different
                            float RandomAggressionFactor = FMath::RandRange(0.8f, 1.2f);
                            AIController->SetAggressionFactor(RandomAggressionFactor);
                        }

                        // Create the arrow widget for this enemy
                        if (ARen_Low_Poly_Character* Player = Cast<ARen_Low_Poly_Character>(SafeThis->GetWorld()->GetFirstPlayerController()->GetPawn()))
                        {
                            if (Player && Player->EnemyArrowWidgetClass)
                            {
                                // Create the arrow widget for the enemy
                                UEnemy_Detection_Arrow* NewArrowWidget = CreateWidget<UEnemy_Detection_Arrow>(SafeThis->GetWorld(), Player->EnemyArrowWidgetClass);
                                if (NewArrowWidget)
                                {
                                    // Add to the viewport
                                    NewArrowWidget->AddToViewport();

                                    // Store the widget in a map for later reference 
                                    Player->EnemyArrowMap.Add(SpawnedEnemy, NewArrowWidget);
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Error, TEXT("Failed to create arrow widget for spawned enemy."));
                                }
                            }
                        }
                    }
                }

                // If all enemies have been spawned, mark spawning as done
                if (i == EnemiesToSpawn - 1)
                {
                    SafeThis->bIsSpawningEnemies = false;

                    // Update enemy behavior parameters after all enemies are spawned
                    SafeThis->UpdateEnemyBehavior();
                }
            }, i * LocalSpawnDelay, false);

        // IMPORTANT: Track this timer handle for cleanup during shutdown
        ActiveTimerHandles.Add(LocalSpawnTimerHandle);
    }
}




void ALowPoly_Survival_GameMode::StartNextRound()
{
    // If we haven't shown the objective message yet, don't start spawning
    if (!bHasShownObjectiveMessage)
    {
        return;
    }

    // Reset game over flag
    bIsGameOver = false;

    // Rest of your existing code...
    SpawnedEnemies.Empty();
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALowPoly_Survival_GameMode::SpawnEnemies, RoundDelay, false);
    bIsSpawningEnemies = true;
    bIsPowerUpSpawned = false;
    NextSpawnRound = FMath::RandRange(3, 5);


}




void ALowPoly_Survival_GameMode::CheckForNextRound()
{

    if (!bHasShownObjectiveMessage)
        return;

    // Don't show round complete if the game is over (which happens when player dies)
    if (bIsGameOver)
        return;

    // Check if the player is dead
    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter && PlayerCharacter->bIsDead)
        return;


    bool bAllEnemiesDead = true;
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (Enemy && !Enemy->bIsDead)
        {
            bAllEnemiesDead = false;
            break;
        }
    }

    if (bAllEnemiesDead && !bIsSpawningEnemies)
    {
        // Reset the attack cycle before moving to the next round
        ResetAttackCycle();

        // Move to the next round
        CurrentRound++;

        ShowRoundCompleteMessage();


        GetWorld()->GetTimerManager().SetTimer(CheckPowerUp, this, &ALowPoly_Survival_GameMode::CheckIfCanPowerUp, 1.0f, false, 5.0f);


        StartNextRound();
    }
}



void ALowPoly_Survival_GameMode::CheckIfCanPowerUp()
{


    // Don't activate power-ups if the game is over
    if (bIsGameOver)
    {
        return;
    }

    // Check if it's time to activate a power-up
    if (CurrentRound % NextSpawnRound == 0)
    {
        ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        if (PlayerCharacter && PlayerController && !PlayerCharacter->bIsDead)  // Add death check
        {

            PlayerCharacter->InterruptCurrentAnimation();
            PlayerCharacter->ClearCurrentAction();
            PlayerCharacter->ActionQueue.Empty();

            // Force any state flags to be reset
            PlayerCharacter->bUsingItem = false;
            PlayerCharacter->bPerformingTechnique = false;
            PlayerCharacter->bPerformingElemental = false;
            PlayerCharacter->bPerformingAbility = false;



            // First, force close the command menu if it's open
            if (PlayerCharacter->CommandMenuWidget && PlayerCharacter->CommandMenuWidget->WidgetSwitcher)
            {
                PlayerCharacter->CommandMenuWidget->WidgetSwitcher->SetActiveWidgetIndex(0);
                PlayerCharacter->bIsInUIMode = false;
                PlayerCharacter->SetInputModeForGameplay();
                PlayerCharacter->ExitCommandMode();
            }

            PlayerController->DisableInput(PlayerController);
            PlayerController->SetViewTargetWithBlend(PlayerCharacter->PowerUpCamera->GetChildActor(), 0.6f, EViewTargetBlendFunction::VTBlend_Linear, 0.0, false);
            GetWorld()->GetTimerManager().SetTimer(PowerUpAnimTimer, this, &ALowPoly_Survival_GameMode::PlayPowerUpAnim, 1.0f, false, 0.5f);
        }
    }


}




void ALowPoly_Survival_GameMode::ActivateRandomPowerUp()
{

    // First check if player character exists and is alive
    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!PlayerCharacter || PlayerCharacter->bIsDead)
    {
        return;
    }

    // Ensure the power-up hasn't been spawned yet in the current round
    if (bIsPowerUpSpawned) return;

    // Set the power-up as spawned
    bIsPowerUpSpawned = true;

    // Randomly pick a power-up from the enum
    ESpecialPowerUp RandomPowerUp = static_cast<ESpecialPowerUp>(FMath::RandRange(0, static_cast<int32>(ESpecialPowerUp::Max) - 1));

    // Log the power-up activation for debugging
    UE_LOG(LogTemp, Warning, TEXT("Activating Power-Up: %s"), *UEnum::GetValueAsString(RandomPowerUp));

    // Add notification for power-up activation
    if (PlayerCharacter->NotificationWidget)
    {
        FString NotificationMessage = FString::Printf(TEXT("%s!"),
            *UEnum::GetValueAsString(RandomPowerUp));

        NotificationMessage.RemoveFromStart(TEXT("ESpecialPowerUp::"));

        PlayerCharacter->NotificationWidget->AddNotification(NotificationMessage, 5.0f);
    }

    PlayerCharacter->ApplyPowerUp(RandomPowerUp);
    PlayerCharacter->ActivatePowerUpVFX();
}




void ALowPoly_Survival_GameMode::PlayPowerUpAnim()
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (!PlayerCharacter || !PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get PlayerCharacter or PlayerController"));
        return;
    }

    PlayerCharacter->bPowerUpPending = true;

    // Clear any current animations and states
    if (UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Stop(0.1f); // Quick stop of any current animation
    }
    PlayerCharacter->ClearCurrentAction(); // Clear any current action states

    // Disable input immediately
    PlayerController->DisableInput(PlayerController);

    // Set powering up state
    PlayerCharacter->bIsPoweringUp = true;

    // Empty the action queue since we're interrupting everything
    PlayerCharacter->ActionQueue.Empty();

    // Freeze all spawned enemies
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (Enemy && !Enemy->bIsDead)
        {
            Enemy->CustomTimeDilation = 0.0f;
        }
    }

    if (PlayerCharacter->PowerUpAnim)
    {
        if (UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance())
        {
            FOnMontageEnded EndDelegate;
            EndDelegate.BindLambda([this, PlayerCharacter, PlayerController](UAnimMontage* Montage, bool bInterrupted)
                {
                    if (!PlayerCharacter)
                    {
                        return;
                    }

                    PlayerCharacter->bIsPoweringUp = false;
                    PlayerCharacter->bPowerUpPending = false;

                    // Return camera after animation ends
                    this->ReturnCamera();

                    // Unfreeze enemies
                    for (AEnemy_Poly* Enemy : SpawnedEnemies)
                    {
                        if (Enemy && !Enemy->bIsDead)
                        {
                            Enemy->CustomTimeDilation = 1.0f;
                        }
                    }

                    // Re-enable input after camera transition
                    FTimerHandle InputTimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(
                        InputTimerHandle,
                        [PlayerController]()
                        {
                            if (PlayerController)
                            {
                                PlayerController->EnableInput(PlayerController);

                            }
                        },
                        0.7f,
                            false
                            );
                });


            // Play the power up animation immediately
            float Duration = AnimInstance->Montage_Play(PlayerCharacter->PowerUpAnim);
            if (Duration > 0.0f)
            {
                AnimInstance->Montage_SetEndDelegate(EndDelegate, PlayerCharacter->PowerUpAnim);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to play PowerUpAnim"));
                PlayerController->EnableInput(PlayerController);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PowerUpAnimMontage is not set on PlayerCharacter!"));
        PlayerController->EnableInput(PlayerController);
    }
}






void ALowPoly_Survival_GameMode::ReturnCamera()
{

    // Unfreeze all enemies
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (Enemy && !Enemy->bIsDead)
        {
            Enemy->CustomTimeDilation = 1.0f;
        }
    }

    //add set view target with blend function here and then add a delay to call the playpowerupanim() using a settimer.
    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerCharacter && PlayerController)

    {

        PlayerController->SetViewTargetWithBlend(PlayerCharacter, 0.6f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);

    }


}






TSubclassOf<AEnemy_Poly> ALowPoly_Survival_GameMode::GetEnemyClassForCurrentRound()
{
    // For rounds 1-4, only spawn spiders
    if (CurrentRound >= 1 && CurrentRound <= 4)
    {
        return BP_Spider;  // Reference to BP_Spider blueprint
    }

    // For rounds 5-9, spawn spiders and wolves
    if (CurrentRound >= 5 && CurrentRound <= 9)
    {
        return FMath::RandBool() ? BP_Spider : BP_Wolf;  // Randomly choose between Spider and Wolf
    }

    // For rounds 10 and above, spawn spiders, wolves, and rock trolls
    if (CurrentRound >= 10)
    {
        int32 RandomChoice = FMath::RandRange(0, 2);
        switch (RandomChoice)
        {
        case 0:
            return BP_Spider;
        case 1:
            return BP_Wolf;
        case 2:
            return BP_RockTroll;

        default:
            return BP_Spider;
        }
    }

    // Default to spawning spiders if something goes wrong
    return BP_Spider;
}



void ALowPoly_Survival_GameMode::ShowObjectiveMessage()
{

    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (!PlayerCharacter || !PlayerController)
    {
        // If we can't get the player character or controller, skip the sequence and start the game
        StartGameAfterObjective();
        return;
    }

    // Use the ObjectiveCamera from the player character
    AActor* TargetCamera = nullptr;

    // Try to use the ObjectiveCamera that's attached to the player
    if (PlayerCharacter->ObjectiveCamera && PlayerCharacter->ObjectiveCamera->GetChildActor())
    {
        TargetCamera = PlayerCharacter->ObjectiveCamera->GetChildActor();
    }

   

    // Set view target to objective camera with blend
    if (TargetCamera)
    {
        PlayerController->SetViewTargetWithBlend(TargetCamera, 0.6f, EViewTargetBlendFunction::VTBlend_Linear, 0.0, false);
        // Disable all input comprehensively
        PlayerController->DisableInput(PlayerController);
        PlayerController->bEnableClickEvents = false;

        PlayerController->bShowMouseCursor = false;
        PlayerController->bEnableMouseOverEvents = false;
    }

    // Create and display the objective message widget
    if (ObjectiveMessageWidgetClass)
    {
        ObjectiveMessageWidget = CreateWidget<UObjective_Message_Widget>(GetWorld(), ObjectiveMessageWidgetClass);

        if (ObjectiveMessageWidget)
        {

            // Add to viewport and play show animation
            ObjectiveMessageWidget->AddToViewport();
            ObjectiveMessageWidget->PlayShowAnimation();

            // Schedule hiding the message after 3 seconds
            GetWorld()->GetTimerManager().SetTimer(
                ObjectiveDisplayTimer,
                this,
                &ALowPoly_Survival_GameMode::HideObjectiveMessage,
                5.0f,  // Show message for 3 seconds
                false
            );
        }
        else
        {
            // If widget creation fails, skip to game start
            StartGameAfterObjective();
        }
    }

    else
    {
        // If no widget class is set, skip to game start
        StartGameAfterObjective();
    }


}



void ALowPoly_Survival_GameMode::HideObjectiveMessage()
{

    // Play hide animation on widget
    if (ObjectiveMessageWidget)
    {
        ObjectiveMessageWidget->PlayHideAnimation();

        // Schedule removing the widget and starting the game after the animation
        // This assumes the animation takes about 1 second to play
        FTimerHandle RemoveWidgetTimer;
        GetWorld()->GetTimerManager().SetTimer(
            RemoveWidgetTimer,
            [this]()
            {
                if (ObjectiveMessageWidget)
                {
                    ObjectiveMessageWidget->RemoveFromParent();
                    ObjectiveMessageWidget = nullptr;
                }

                // Return camera to player and start game
                this->StartGameAfterObjective();
            },
            1.0f,  // Delay to allow animation to complete
                false
                );
    }
    else
    {
        // If widget is not valid, just start the game
        StartGameAfterObjective();
    }

}



void ALowPoly_Survival_GameMode::StartGameAfterObjective()
{

    // Return camera to player character
    ARen_Low_Poly_Character* PlayerCharacter = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerCharacter && PlayerController)
    {
        PlayerController->SetViewTargetWithBlend(PlayerCharacter, 0.6f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);

        // Add debug message before enabling input
        UE_LOG(LogTemp, Warning, TEXT("About to re-enable input in StartGameAfterObjective"));
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Re-enabling input..."));

        // Reset any UI flags on the character
        PlayerCharacter->bIsInUIMode = false;

        FTimerHandle InputEnableTimer;

        // Add a slight delay before re-enabling input to ensure all transitions are complete
        GetWorld()->GetTimerManager().SetTimer(
            InputEnableTimer,
            [PlayerController]()
            {
                if (PlayerController && PlayerController->IsValidLowLevel())
                {
                    // Re-enable all input types
                    PlayerController->EnableInput(PlayerController);
                    PlayerController->SetIgnoreLookInput(false);
                    PlayerController->SetIgnoreMoveInput(false);

                    // Explicitly set game-only input mode
                    FInputModeGameOnly GameOnlyMode;
                    PlayerController->SetInputMode(GameOnlyMode);

                    // Restore mouse settings
                    PlayerController->bShowMouseCursor = false;
                    PlayerController->bEnableMouseOverEvents = true;
                }
            },
            1.0f,
                false
                );
    }

    if (LevelMusic)
    {
        // Play the music with fade-in
        LevelMusicComponent = UGameplayStatics::SpawnSound2D(this, LevelMusic);
        if (LevelMusicComponent)
        {
            // Get the master volume from game settings
            float MasterVolume = 1.0f;
            if (UGame_Instance* GameInstance = Cast<UGame_Instance>(GetGameInstance()))
            {
                MasterVolume = GameInstance->GameSettings.MasterVolume;
            }

            // Apply the master volume
            LevelMusicComponent->SetVolumeMultiplier(MasterVolume);

            // Then fade in from silence
            LevelMusicComponent->FadeIn(2.0f, MasterVolume);

            // Make sure it doesn't automatically destroy when sound finishes
            LevelMusicComponent->bAutoDestroy = false;
        }
    }

    // Mark that we've shown the objective
    bHasShownObjectiveMessage = true;

    // Start the first round
    StartNextRound();

}




void ALowPoly_Survival_GameMode::ShowRoundCompleteMessage()
{

    // Create and display the round complete message widget
    if (RoundCompleteWidgetClass)
    {
        RoundCompleteWidget = CreateWidget<URound_Complete_Message_Widget>(GetWorld(), RoundCompleteWidgetClass);

        if (RoundCompleteWidget)
        {
            // Add to viewport and play show animation
            RoundCompleteWidget->AddToViewport();
            RoundCompleteWidget->PlayShowAnimation();

            // Schedule hiding the message after 4 seconds
            GetWorld()->GetTimerManager().SetTimer(
                RoundCompleteDisplayTimer,
                this,
                &ALowPoly_Survival_GameMode::HideRoundCompleteMessage,
                3.0f,  // Show message for 4 seconds
                false
            );
        }
    }



}

void ALowPoly_Survival_GameMode::HideRoundCompleteMessage()
{


    // Play hide animation on widget
    if (RoundCompleteWidget)
    {
        RoundCompleteWidget->PlayHideAnimation();

        // Use a weak pointer to avoid dangling references
        TWeakObjectPtr<ALowPoly_Survival_GameMode> WeakThis = this;
        TWeakObjectPtr<URound_Complete_Message_Widget> WeakWidget = RoundCompleteWidget;

        // Schedule removing the widget after the animation
        FTimerHandle RemoveWidgetTimer;
        GetWorld()->GetTimerManager().SetTimer(
            RemoveWidgetTimer,
            [WeakThis, WeakWidget]()
            {
                // Safety check: Make sure both the game mode and widget are still valid
                if (!WeakThis.IsValid() || WeakThis->bIsShuttingDown)
                {
                    return; // Game mode is being destroyed, abort
                }

                // Check if widget is still valid before attempting to remove it
                if (WeakWidget.IsValid())
                {
                    WeakWidget->RemoveFromParent();

                    // Only set to nullptr if the game mode is valid
                    if (WeakThis.IsValid())
                    {
                        WeakThis->RoundCompleteWidget = nullptr;
                    }
                }
            },
            1.0f,  // Delay to allow animation to complete
            false
        );

        // IMPORTANT: Track this timer handle for cleanup during shutdown
        ActiveTimerHandles.Add(RemoveWidgetTimer);
    }

}




void ALowPoly_Survival_GameMode::OnEnemyDestroyed()
{

    for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; i--)
    {
        if (SpawnedEnemies.IsValidIndex(i) && SpawnedEnemies[i] && SpawnedEnemies[i]->bIsDead)
        {
            SpawnedEnemies.RemoveAt(i);
        }
    }

    // Update behavior parameters for remaining enemies
    UpdateEnemyBehavior();

    // After removing dead enemies, check for the next round
    CheckForNextRound();

}




FVector ALowPoly_Survival_GameMode::GetRandomPointNearPlayer()
{
    // Check if we have any spawn zones in the array
    if (SpawnZones.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No spawn zones found!"));
        return FVector::ZeroVector;  // Return default if no spawn zones are available
    }

    // Pick a random spawn zone actor from the list
    AActor* RandomSpawnZone = SpawnZones[FMath::RandRange(0, SpawnZones.Num() - 1)];

    // Define a radius for the spawn area around the spawn zone actor
    float SpawnRadiuss = 500.0f;  // Adjust this radius as needed

    // Get the spawn zone's location
    FVector SpawnZoneLocation = RandomSpawnZone->GetActorLocation();

    // Generate a random point within a circle defined by SpawnRadius
    float RandomX = FMath::RandRange(SpawnZoneLocation.X - SpawnRadiuss, SpawnZoneLocation.X + SpawnRadiuss);
    float RandomY = FMath::RandRange(SpawnZoneLocation.Y - SpawnRadiuss, SpawnZoneLocation.Y + SpawnRadiuss);

    // You could also optionally keep Z coordinate similar or random within a vertical range
    float RandomZ = SpawnZoneLocation.Z;

    // Return a random point within the spawn zone area
    return FVector(RandomX, RandomY, RandomZ);
}




void ALowPoly_Survival_GameMode::OnSteamOverlayActivated(bool bIsActive)
{

    if (bIsActive)
    {
        // Pause the game when overlay is activated
        SetPause(GetWorld()->GetFirstPlayerController());
        GEngine->AddOnScreenDebugMessage(2, 2.5f, FColor::Green, TEXT("Overlay activated!"));
    }
    else
    {
        // Unpause when overlay is deactivated
        ClearPause();
        GEngine->AddOnScreenDebugMessage(2, 2.5f, FColor::Green, TEXT("Overlay deactivated!"));
    }


}





void ALowPoly_Survival_GameMode::SetupSteamOverlayDetection()
{

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get("Steam");
    if (OnlineSubsystem)
    {
        IOnlineExternalUIPtr ExternalUI = OnlineSubsystem->GetExternalUIInterface();
        if (ExternalUI.IsValid())
        {
            OnOverlayActivatedDelegateHandle = ExternalUI->AddOnExternalUIChangeDelegate_Handle(
                FOnExternalUIChangeDelegate::CreateUObject(this, &ALowPoly_Survival_GameMode::OnSteamOverlayActivated));
        }
    }



}




int32 ALowPoly_Survival_GameMode::CalculatePointsForEnemy(FScore_Struct EnemyScoreStruct)
{

    //Calculate points by multiplying base points with the round multiplier
    return FMath::RoundToInt(EnemyScoreStruct.BasePoints * EnemyScoreStruct.RoundMultiplier * CurrentRound);

}



void ALowPoly_Survival_GameMode::AssignEnemyNumbers()
{

    // Remove any destroyed enemies from the ActiveEnemies array
    ActiveEnemies.RemoveAll([](AEnemy_AIController* EnemyAI) {
        return !EnemyAI || !EnemyAI->GetPawn() || EnemyAI->GetPawn()->IsPendingKillPending();
        });

    int32 MaxNumber = ActiveEnemies.Num();

    // Shuffle or sort enemies randomly
    ActiveEnemies.Sort([](const AEnemy_AIController& A, const AEnemy_AIController& B) {
        return FMath::RandBool();
        });

    // Assign numbers (1 is the highest priority)
    for (int32 i = 0; i < ActiveEnemies.Num(); i++)
    {
        if (ActiveEnemies[i])
        {
            ActiveEnemies[i]->SetEnemyNumber(MaxNumber - i);  // Highest number first
        }
    }

}

void ALowPoly_Survival_GameMode::CycleToNextEnemy()
{

    // Ensure the current attacker is valid and hasn't been destroyed
    if (CurrentAttacker && CurrentAttacker->GetPawn())
    {
        int32 CurrentAttackerIndex = SpawnedEnemies.IndexOfByKey(CurrentAttacker->GetPawn());

        if (CurrentAttackerIndex != INDEX_NONE)
        {
            // Cycle to the next attacker, limit the number of attempts to avoid infinite recursion
            int32 NumAttempts = 0;
            int32 NextAttackerIndex;
            AEnemy_AIController* NextAttacker;

            do
            {
                NextAttackerIndex = (CurrentAttackerIndex + 1) % SpawnedEnemies.Num();
                NextAttacker = Cast<AEnemy_AIController>(SpawnedEnemies[NextAttackerIndex]->GetController());

                // Break if we find a valid attacker
                if (NextAttacker && NextAttacker->GetPawn())
                {
                    CurrentAttacker = NextAttacker;
                    CurrentAttacker->AttackPlayer();
                    return; // Exit the function once we find a valid attacker
                }

                CurrentAttackerIndex = NextAttackerIndex;
                NumAttempts++;

            } while (NumAttempts < SpawnedEnemies.Num());  // Avoid infinite recursion by limiting to number of enemies

            // If no valid attackers are found after looping through all enemies, reset the cycle
            ResetAttackCycle();
        }
    }
    else
    {
        // If the current attacker is destroyed, try cycling to the next one
        ResetAttackCycle();
    }

}

void ALowPoly_Survival_GameMode::ResetAttackCycle()
{

    // Reset the current attacker
    CurrentAttacker = nullptr;


}

void ALowPoly_Survival_GameMode::StartEnemyAttackCycle()
{

    if (!bIsSpawningEnemies && SpawnedEnemies.Num() > 0)
    {
        // Start the cycle since all enemies have been spawned
        CycleToNextEnemy();  // Call your logic for cycling enemies here
    }


}
