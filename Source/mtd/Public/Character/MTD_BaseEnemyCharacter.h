#pragma once

#include "mtd.h"
#include "MTD_BaseCharacter.h"

#include "MTD_BaseEnemyCharacter.generated.h"

class UBehaviorTree;
class UBoxComponent;
class UMTD_EnemyData;
class UMTD_EnemyExtensionComponent;
class USphereComponent;

UCLASS()
class MTD_API AMTD_BaseEnemyCharacter : public AMTD_BaseCharacter
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewTargetSignature, AActor*, OldTarget, AActor*, NewTarget);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSignature);

public:
    AMTD_BaseEnemyCharacter();

    UBehaviorTree *GetBehaviorTree() const;

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    //~End of AActor Interface

    //~AMTD_BaseCharacter Interface
    virtual void InitializeAttributes() override;
    virtual void OnDeathStarted_Implementation(AActor *OwningActor) override;
    virtual void OnDeathFinished_Implementation(AActor *OwningActor) override;
    //~End of AMTD_BaseCharacter Interface

    UFUNCTION(BlueprintNativeEvent)
    void OnHealthChanged(UMTD_HealthComponent *InHealthComponent, float OldValue, float NewValue, AActor *InInstigator);
    virtual void OnHealthChanged_Implementation(
        UMTD_HealthComponent *InHealthComponent, float OldValue, float NewValue, AActor *InInstigator);

    
    //~IMTD_GameResultInterface Interface
    virtual void OnGameTerminated_Implementation(EMTD_GameResult GameResult) override;
    //~End of IMTD_GameResultInterface Interface
    
private:
    void EquipDefaultWeapon();

    void SetNewTarget(APawn *Pawn);
    APawn *GetClosestTarget();
    bool IsActorInRedirectRange(const APawn *Pawn) const;

    UFUNCTION()
    void OnTargetDied(AActor *Actor);

    /**
     * Compute what's the cheapiest actor to travel towards cost wise.
     * Note: It's a heavy task, use the function carefully.
     */
    AActor *GetCheapiestActor(AActor *Lhs, AActor *Rhs) const;

    void UnlockRetarget();
    void DisableCollisions();

    UFUNCTION()
    void OnSightSphereBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    UFUNCTION()
    void OnLoseSightSphereEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);

    UFUNCTION()
    void OnAttackTriggerBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    UFUNCTION()
    void OnAttackTriggerEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);

public:
    UPROPERTY(BlueprintAssignable)
    FDynamicMulticastSignature OnStartAttackingDelegate;

    UPROPERTY(BlueprintAssignable)
    FDynamicMulticastSignature OnStopAttackingDelegate;

    UPROPERTY(BlueprintAssignable)
    FOnNewTargetSignature OnNewTargetDelegate;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Enemy", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> SightSphere = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Enemy", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> LoseSightSphere = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Enemy", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> AttackTrigger = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Enemy", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_EnemyExtensionComponent> EnemyExtensionComponent = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Enemy", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

    /** Equipment the enemy will be spawned with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Enemy", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UMTD_EquipmentDefinition> DefaultWeaponDefinitionClass = nullptr;

    UPROPERTY()
    TObjectPtr<APawn> Target = nullptr;

    UPROPERTY()
    TArray<TObjectPtr<APawn>> DetectedTargets;

    UPROPERTY()
    TArray<TObjectPtr<APawn>> AttackTargets;

    /** Can retarget? */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Enemy|Retarget|Runtime")
    bool bRetargetLock = false;

    /** Seconds the enemy will not be able to retarget on his own will after such a retarget. */
    UPROPERTY(EditAnywhere, Category="MTD|Enemy|Retarget", meta=(ClampMin="0.1"))
    float TimeToUnlockRetarget = 0.1f;

    /** Units the damaging actor must be in to be retargetted to. */
    UPROPERTY(EditAnywhere, Category="MTD|Enemy|Retarget")
    float RetargetMaxRange = 500.f;
};

inline UBehaviorTree *AMTD_BaseEnemyCharacter::GetBehaviorTree() const
{
    return BehaviorTree;
}

inline void AMTD_BaseEnemyCharacter::UnlockRetarget()
{
    bRetargetLock = false;
}
