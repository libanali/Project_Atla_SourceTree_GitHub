// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy_AIController.generated.h"


class AEnemy_Token_Manager;
class AEnemy_Poly;
/**
 * 
 */

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
    Spider UMETA(DisplayName = "Spider"),
    Wolf UMETA(DisplayName = "Wolf"),
    RockTroll UMETA(DisplayName = "Rock Troll")
};


UCLASS()
class PROJECT_ATLA_API AEnemy_AIController : public AAIController
{
    GENERATED_BODY()


public:

    AEnemy_AIController();
    

    virtual void BeginPlay() override;

    void SetTarget(AActor* NewTarget);
    void AttackPlayer();
    void StrafeAroundPlayer();
    void FacePlayer();
    void ResetAttackCooldown();
    void UpdateHitState();
    void DisableAI();
    void RestartAI();
    void ApplyDirectApproach(float DeltaTime);
    float DistanceToPlayer;
    float AttackRange;

    // Function to update the behavior of the enemy
   // void UpdateBehaviour();

    // Function to set the enemy's assigned number
    void SetEnemyNumber(int32 NewNumber);

    void SetFrozenState(bool bFrozen) { bIsFrozen = bFrozen; }
    bool IsFrozen() const { return bIsFrozen; }


    bool bIsFrozen;
    bool bIsStunned;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsAIStopped;

   AEnemy_Poly* TheEnemy;


    // Function to get the enemy's current assigned number
    int32 GetEnemyNumber() const;

    int32 EnemyNumber;
    bool bIsAttacking;
    bool bIsClosingIn;
    bool bIsInAttackRange;
    bool bIsPetrified;

    FTimerHandle PetrifiedEffectTimer;


    // Getter/setter for enemy type
    EEnemyType GetEnemyType() const;
    void SetEnemyType(EEnemyType NewType);

    // Initialize based on enemy type
    void InitializeForEnemyType(EEnemyType Type);

    // Set total enemy count (for spacing calculation)
    void SetTotalEnemyCount(int32 Count);

    // Set attack probability modifiers
    void SetBaseAttackProbability(float Probability);
    void SetAggressionFactor(float Factor);

    // Update behavior with tick delta time
    void UpdateBehaviour(float DeltaTime);

    // Existing methods you already have
    // ...

    // Add this variable to your existing class
    EEnemyType EnemyType = EEnemyType::Spider;  // Default type


 
private:
    AActor* TargetPlayer;
    FTimerHandle AttackDelayHandle;
    bool bIsAttackOnCooldown;  // To check if attack is cooling down
    float LastDecisionTime;  // Tracks the time of the last movement decision
    float DecisionInterval;  // The time interval between decisions
    bool bIsStrafing;        // Tracks if the AI is currently strafing
  
    // Attack probability system
    float AttackProbability = 0.0f;
    float BaseAttackProbability = 0.5f;  // Base chance to attack per second
    float AttackProbabilityMultiplier = 1.0f;  // Can be modified based on difficulty
    float AggressionFactor = 1.0f;  // Individual enemy aggression variation

    // Movement pattern tracking
    float MovementPatternTimer = 0.0f;
    FVector CurrentMoveTarget;
    int32 TotalEnemyCount = 1;

    // Movement pattern methods
    void ApplyErraticMovement(float DeltaTime);  // Spider movement
    void ApplyCirclingMovement(float DeltaTime); // Wolf movement
    void ApplyDirectMovement(float DeltaTime);   // Rock Troll movement
    void PrepareForAttack(EEnemyType TheEnemyType, float DeltaTime);
    float GetAttackRateForEnemyType(EEnemyType TheEnemyType);



    // Anti-crowding system
    static int32 CurrentEngagedEnemies;
    static const int32 MaxEngagedEnemies;
    bool bIsEngaged;
    bool bCanEngage;

    // Helper function
    void CheckEngagementStatus();

protected:

    virtual void Tick(float deltaTime);
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


};
