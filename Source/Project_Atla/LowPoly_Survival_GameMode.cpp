// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_Survival_GameMode.h"
#include "Enemy.h"
#include "Ren_Low_Poly_Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Enemy_Token_Manager.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"


ALowPoly_Survival_GameMode::ALowPoly_Survival_GameMode()
{

    RoundDelay = 2.5f;
    BaseEnemiesPerRound = 3;
    SpawnRadius = 1200.0f;
    CurrentRound = 21;
    AdditionalEnemyHealthPerRound = 30.0f;
    AdditionalEnemiesPerRound = 1.9f;
    BaseSpawnDelay = 2.0f;         // Initial delay between spawns in the first round
    MinSpawnDelay = 1.5f;          // Minimum delay between spawns in later rounds
    DelayDecreasePerRound = 0.1f;
    bIsSpawningEnemies = false;
    bIsPowerUpSpawned = false;
    bStopSpawning = false;



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

    // Destroy all spawned enemies
    for (AEnemy_Poly* Enemy : SpawnedEnemies)
    {
        if (Enemy && !Enemy->IsPendingKillPending())
        {
            Enemy->Destroy();
        }
    }

    SpawnedEnemies.Empty();

}

void ALowPoly_Survival_GameMode::UpdateEnemyBehavior()
{

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


void ALowPoly_Survival_GameMode::BeginPlay()
{

    Super::BeginPlay();

    StartNextRound();

    GetWorld()->GetTimerManager().SetTimer(NumberUpdateTimer, this, &ALowPoly_Survival_GameMode::UpdateEnemyNumbers, 5.0f, true);

    GetWorld()->GetTimerManager().SetTimer(
        EnemyBehaviorUpdateTimer,
        this,
        &ALowPoly_Survival_GameMode::UpdateEnemyBehavior,
        5.0f,
        true
    );
    SpecialEventInterval = FMath::RandRange(1, 3);
    // Find all actors in the level with the tag "SpawnZone"
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawn Zone"), FoundActors);

    // Store these actors as spawn zones
    SpawnZones.Empty();
    for (AActor* Actor : FoundActors)
    {
        if (Actor)
        {
            SpawnZones.Add(Actor);
        }
    }
}



void ALowPoly_Survival_GameMode::Tick(float DeltaTime)
{

    OnEnemyDestroyed();
}



void ALowPoly_Survival_GameMode::SpawnEnemies()
{  

    // Early return if spawning is disabled
    if (bStopSpawning)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawning is disabled. Aborting SpawnEnemies."));
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
        FTimerHandle LocalSpawnTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(LocalSpawnTimerHandle, [this, i, EnemiesToSpawn]()
            {
                FVector SpawnLocation = GetRandomPointNearPlayer();
                FRotator SpawnRotation = FRotator::ZeroRotator;

                // Add a small offset to the spawn location based on the enemy index to space them out
                float SpawnOffset = 200.0f; // Adjust this value to control spacing
                SpawnLocation.X += i * SpawnOffset;
                SpawnLocation.Y += i * SpawnOffset;

                TSubclassOf<AEnemy_Poly> EnemyToSpawnClass = GetEnemyClassForCurrentRound();

                if (EnemyToSpawnClass)
                {
                    AEnemy_Poly* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy_Poly>(EnemyToSpawnClass, SpawnLocation, SpawnRotation);
                    if (SpawnedEnemy)
                    {
                        UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s"), *SpawnedEnemy->GetName());

                        // Increase enemy health based on the current round
                        float AddedEnemyHealth = SpawnedEnemy->MaxEnemyHealth + (CurrentRound - 1) * AdditionalEnemyHealthPerRound;
                        SpawnedEnemy->IncreaseEnemyHealth(AddedEnemyHealth, true);

                        // Add the spawned enemy to the list
                        SpawnedEnemies.Add(SpawnedEnemy);

                        // Initialize the AI controller with the appropriate enemy type
                        AEnemy_AIController* AIController = Cast<AEnemy_AIController>(SpawnedEnemy->GetController());
                        if (AIController)
                        {
                            // Set enemy type based on class
                            EEnemyType EnemyType;
                            if (EnemyToSpawnClass == BP_Spider)
                            {
                                EnemyType = EEnemyType::Spider;
                            }
                            else if (EnemyToSpawnClass == BP_Wolf)
                            {
                                EnemyType = EEnemyType::Wolf;
                            }
                            else if (EnemyToSpawnClass == BP_RockTroll)
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
                        if (ARen_Low_Poly_Character* Player = Cast<ARen_Low_Poly_Character>(GetWorld()->GetFirstPlayerController()->GetPawn()))
                        {
                            if (Player && Player->EnemyArrowWidgetClass)
                            {
                                // Create the arrow widget for the enemy
                                UEnemy_Detection_Arrow* NewArrowWidget = CreateWidget<UEnemy_Detection_Arrow>(GetWorld(), Player->EnemyArrowWidgetClass);
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
                    bIsSpawningEnemies = false;

                    // Update enemy behavior parameters after all enemies are spawned
                    UpdateEnemyBehavior();
                }

            }, i * LocalSpawnDelay, false);
    }
}




void ALowPoly_Survival_GameMode::StartNextRound()
{
    // Reset game over flag
    bIsGameOver = false;

    // Rest of your existing code...
    SpawnedEnemies.Empty();
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALowPoly_Survival_GameMode::SpawnEnemies, RoundDelay, false);
    bIsSpawningEnemies = true;
    bIsPowerUpSpawned = false;
    NextSpawnRound = FMath::RandRange(1, 1);


}




void ALowPoly_Survival_GameMode::CheckForNextRound()
{
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


        GetWorld()->GetTimerManager().SetTimer(CheckPowerUp, this, &ALowPoly_Survival_GameMode::CheckIfCanPowerUp, 1.0f, false, 2.0f);



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
