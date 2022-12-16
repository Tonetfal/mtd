#pragma once

#include "AIController.h"
#include "Character/MTD_TeamComponent.h"
#include "mtd.h"

#include "MTD_EnemyController.generated.h"

class UBehaviorTreeComponent;
class UMTD_BalanceHitData;
class AMTD_BaseEnemyCharacter;
class UMovementComponent;

UCLASS()
class MTD_API AMTD_EnemyController : public AAIController
{
    GENERATED_BODY()

public:
    AMTD_EnemyController();
    virtual void Tick(float DeltaSeconds) override;

    virtual FGenericTeamId GetGenericTeamId() const override
    {
        return Team->GetGenericTeamId();
    }

protected:
    //~AAIController Interface
    virtual void OnPossess(APawn *InPawn) override;
    //~End of AAIController Interface

    virtual void PostOnPossess();

    UFUNCTION(BlueprintNativeEvent)
    void OnPawnDied(AActor *Actor);
    virtual void OnPawnDied_Implementation(AActor *Actor);

private:
    UFUNCTION()
    void OnNewTarget(AActor *OldTarget, AActor *NewTarget);

    UFUNCTION()
    void OnTargetDied(AActor *Actor);

    UFUNCTION()
    void OnStartAttacking();

    UFUNCTION()
    void OnStopAttacking();

    UFUNCTION()
    void OnKnockback(const UMTD_BalanceHitData *HitData);

    UFUNCTION()
    void ResetKnockback();

    void StartRunningBehaviorTree(AMTD_BaseEnemyCharacter *Enemy);
    void SetupKnockbacks(AMTD_BaseEnemyCharacter *Enemy);

private:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> Team = nullptr;

    UPROPERTY()
    TObjectPtr<UMovementComponent> OwnerMovementComponent = nullptr;

    bool bAttack = false;
    FTimerHandle KnockbackTimerHandle;
};

