#pragma once

#include "Character/MTD_BaseCharacter.h"
#include "mtd.h"

#include "MTD_BaseFoeCharacter.generated.h"

class UBehaviorTree;
class UBoxComponent;
class UMTD_FoeData;
class UMTD_EquipmentDefinition;
class USphereComponent;

/**
 * Base foe character used to create specific ones.
 *
 * List of added behaviors:
 * - Ability system component
 * - Health
 * - Mana
 * - Balance
 * - Melee combat
 * - Custom sight sense
 * - Action decision making
 */
UCLASS()
class MTD_API AMTD_BaseFoeCharacter
    : public AMTD_BaseCharacter
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSignature);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnNewTargetSignature,
        AActor *, OldTarget,
        AActor *, NewTarget);

public:
    AMTD_BaseFoeCharacter();
    
    //~AActor Interface
    virtual void PreInitializeComponents() override;

protected:
    virtual void BeginPlay() override;
    //~End of AActor Interface

    //~AMTD_BaseCharacter Interface
    virtual void InitializeAttributes() override;
    virtual void OnDeathStarted_Implementation(AActor *OwningActor) override;
    virtual void OnDeathFinished_Implementation(AActor *OwningActor) override;
    //~End of AMTD_BaseCharacter Interface
    
    //~IMTD_GameResultInterface Interface
    virtual void OnGameTerminated_Implementation(EMTD_GameResult GameResult) override;
    //~End of IMTD_GameResultInterface Interface

    /**
     * Event to fire when our health has changes.
     * @param   InHealthComponent: health component the health has changed on.
     * @param   OldValue: old health.
     * @param   NewValue: new health.
     * @param   InInstigator: logical actor caused health to change. Probably is a player state.
     */
    UFUNCTION(BlueprintNativeEvent)
    void OnHealthChanged(UMTD_HealthComponent *InHealthComponent, float OldValue, float NewValue, AActor *InInstigator);
    virtual void OnHealthChanged_Implementation(UMTD_HealthComponent *InHealthComponent, float OldValue, float NewValue,
        AActor *InInstigator);
    
private:
    /**
     * Equip default weapon. Does not give any stats, visuals only.
     * @see DefaultWeaponID
     */
    void EquipDefaultWeapon();

    /**
     * Set a new target to attack.
     * @param   InTarget: new pawn to attack.
     */
    void SetNewTarget(APawn *InTarget);

    /**
     * Get the closest target among detected actors.
     *
     * Avoid using this function too often because it's a heavy one, it makes use of path finding algorithms
     */
    APawn *GetClosestTarget();

    /**
     * Compute what's the cheapiest actor to travel towards cost wise.
     * Note: It's a heavy task, use the function carefully.
     */
    AActor *GetCheapiestActor(AActor *Lhs, AActor *Rhs) const;
    
    bool IsActorInRedirectRange(const APawn *Pawn) const;

    /**
     * Event to fire when attack target has died.
     * @param   InTarget: attack target that has died. Will probably be in a death state.
     */
    UFUNCTION()
    void OnTargetDied(AActor *InTarget);

    /**
     * Event to fire when retarget should be unlocked.
     */
    void UnlockRetarget();

protected:
    /**
     * Try to drop everything we can.
     */
    UFUNCTION(BlueprintNativeEvent)
    void DropGoods();
    virtual void DropGoods_Implementation();

private:
    /**
     * Drop an item.
     */
    void DropItem();

    /**
     * Broadcast experience to all present players.
     */
    void DropExp();

    /**
     * Disable all collisions, e.g. capsule collision, sight spheres, and attack triggers.
     */
    void DisableCollisions();

    /**
     * Event to fire when sight sphere has began overlapping with an actor.
     */
    UFUNCTION()
    void OnSightSphereBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    /**
     * Event to fire when lose sight sphere has ended overlapping with an actor.
     */
    UFUNCTION()
    void OnLoseSightSphereEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);

    /**
     * Event to fire when attack trigger has began overlapping with an actor.
     */
    UFUNCTION()
    void OnAttackTriggerBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    /**
     * Event to fire when attack trigger has ended overlapping with an actor.
     */
    UFUNCTION()
    void OnAttackTriggerEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);

public:
    /**
     * Get AI behavior tree.
     * @return  Behavior tree.
     */
    UBehaviorTree *GetBehaviorTree() const;

    /**
     * Get foe data.
     * @return  Foe data.
     */
    const UMTD_FoeData *GetFoeData() const;
    
public:
    /** Delegate to fire when character should enter in attack mode. */
    UPROPERTY(BlueprintAssignable)
    FDynamicMulticastSignature OnStartAttackingDelegate;

    /** Delegate to fire when character should leave in attack mode. */
    UPROPERTY(BlueprintAssignable)
    FDynamicMulticastSignature OnStopAttackingDelegate;

    /** Delegate to fire when attack target has changed. */
    UPROPERTY(BlueprintAssignable)
    FOnNewTargetSignature OnNewTargetDelegate;

private:
    /** Overlap sphere to detect players. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Foe", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> SightSphere = nullptr;

    /** Overlap sphere players have to leave once entered in sight sphere in order to be un-detected by us. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Foe", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> LoseSightSphere = nullptr;

    /** Overlap box that forces us to enter or leave attack mode depending on players presence in it. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Foe", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> AttackTrigger = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Foe", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_FoeData> FoeData = nullptr;

    /** Behavior tree running foe logic. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Foe", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

    /** Equipment the foe will be spawned with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Foe", meta=(AllowPrivateAccess="true"))
    int32 DefaultWeaponID = 0;

    /** Attack target pawn. */
    UPROPERTY()
    TObjectPtr<APawn> Target = nullptr;

    /** Container of detected pawns that can be targetted. */
    UPROPERTY()
    TArray<TObjectPtr<APawn>> DetectedTargets;

    /** Container of pawns inside attack trigger that can be targetted. */
    UPROPERTY()
    TArray<TObjectPtr<APawn>> AttackTargets;

    /**
     * If true, foe can try to change current target, false otherwise. Is needed to avoid running retargetting logic
     * too often.
     */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe|Retarget|Runtime")
    bool bRetargetLock = false;

    /** Time in seconds the foe will not be able to retarget on his own will after a retarget. */
    UPROPERTY(EditAnywhere, Category="MTD|Foe|Retarget", meta=(ClampMin="0.1"))
    float TimeToUnlockRetarget = 0.1f;

    /** Distance the damaging actor must be in to be retargetted to. */
    UPROPERTY(EditAnywhere, Category="MTD|Foe|Retarget")
    float RetargetMaxRange = 500.f;
};

inline void AMTD_BaseFoeCharacter::UnlockRetarget()
{
    bRetargetLock = false;
}

inline UBehaviorTree *AMTD_BaseFoeCharacter::GetBehaviorTree() const
{
    return BehaviorTree;
}

inline const UMTD_FoeData *AMTD_BaseFoeCharacter::GetFoeData() const
{
    return FoeData;
}
