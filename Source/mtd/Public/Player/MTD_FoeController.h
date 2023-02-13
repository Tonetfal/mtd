#pragma once

#include "AIController.h"
#include "Character/MTD_TeamComponent.h"
#include "mtd.h"

#include "MTD_FoeController.generated.h"

class AMTD_BaseFoeCharacter;
class UBehaviorTreeComponent;
class UMovementComponent;
class UMTD_BalanceHitData;
class UMTD_FoeData;

/**
 * Foe controller that gives an owning foe core functions and controls some of its decisions.
 *
 * The core functions the controller grants the foe:
 * - team association
 * - knock backs on behavior tree
 * - smooth rotation
 *
 * @see AMTD_BaseFoeCharacter
 */
UCLASS()
class MTD_API AMTD_FoeController
    : public AAIController
{
    GENERATED_BODY()

public:
    AMTD_FoeController();

protected:
    //~AAIController Interface
    virtual void OnPossess(APawn *InPawn) override;

public:
    virtual FGenericTeamId GetGenericTeamId() const override;
    //~End of AAIController Interface

protected:
    virtual void PostOnPossess();

public:
    //~AActor Interface
    virtual void Tick(float DeltaSeconds) override;
    //~End of AActor Interface

    /**
     * Event to fire when owning foe dies.
     * @param   Actor: foe actor.
     */
    UFUNCTION(BlueprintNativeEvent)
    void OnPawnDied(AActor *Actor);
    virtual void OnPawnDied_Implementation(AActor *Actor);

private:
    /**
     * Event to fire when owning foe selects a new target.
     * @param   OldTarget: previous target.
     * @param   NewTarget: current target.
     */
    UFUNCTION()
    void OnNewTarget(AActor *OldTarget, AActor *NewTarget);

    /**
     * Event to fire when target actor dies.
     * @param   Actor: target actor.
     */
    UFUNCTION()
    void OnTargetDied(AActor *Actor);

    /**
     * Event to fire when owning foe demands to start attacking.
     */
    UFUNCTION()
    void OnStartAttacking();

    /**
     * Event to fire when owning foe demands to stop attacking.
     */
    UFUNCTION()
    void OnStopAttacking();

    /**
     * Event to fire when owning foe is knocked back.
     * @param   HitData: data about knock back.
     */
    UFUNCTION()
    void OnKnockBack(const UMTD_BalanceHitData *HitData);

    /**
     * Event to fire when owning foe shouldn't be knocked back anymore.
     */
    UFUNCTION()
    void ResetKnockBack();

    /**
     * Start running behavior tree logic on owning foe character.
     * @param   OwningCharacter: owning foe casted to MTD class.
     */
    void StartRunningBehaviorTree(const AMTD_BaseFoeCharacter *OwningCharacter);
    
    /**
     * Setup knock backs data on owning foe character.
     * @param   OwningCharacter: owning foe casted to MTD class.
     */
    void SetupKnockBacks(const AMTD_BaseFoeCharacter *OwningCharacter);

private:
    /** Behavior tree component to run AI logic. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent = nullptr;

    /** Component that associates us to a team. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> Team = nullptr;

    /** Cached owning foe movement component. */
    UPROPERTY()
    TObjectPtr<UMovementComponent> OwnerMovementComponent = nullptr;
    
    /** Cached owning foe data. */
    UPROPERTY()
    TObjectPtr<const UMTD_FoeData> FoeData = nullptr;

    /** If true, foe is attacking, false otherwise. */
    bool bAttack = false;

    /** Timer handle to manage knock backs. */
    FTimerHandle KnockBackTimerHandle;
};

inline FGenericTeamId AMTD_FoeController::GetGenericTeamId() const
{
    return Team->GetGenericTeamId();
}
