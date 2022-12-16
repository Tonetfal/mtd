#include "Player/MTD_EnemyController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/MTD_BalanceComponent.h"
#include "Character/MTD_BaseEnemyCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_EnemyExtensionComponent.h"
#include "Character/MTD_HealthComponent.h"
#include "Character/MTD_TeamComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/MTD_Utility.h"

AMTD_EnemyController::AMTD_EnemyController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
    Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
    Team = CreateDefaultSubobject<UMTD_TeamComponent>(TEXT("MTD Team Component"));

    bAttachToPawn = true;
    bWantsPlayerState = true;
}

void AMTD_EnemyController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const AActor *FocusActor = GetFocusActor();
    if ((!IsValid(FocusActor)) && (IsValid(OwnerMovementComponent)) && (!OwnerMovementComponent->Velocity.IsZero()))
    {
        const FRotator DirRot = OwnerMovementComponent->Velocity.GetSafeNormal().ToOrientationRotator();
        SetControlRotation(DirRot);
    }
}

void AMTD_EnemyController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    auto Enemy = CastChecked<AMTD_BaseEnemyCharacter>(InPawn);
    Enemy->OnNewTargetDelegate.AddDynamic(this, &ThisClass::OnNewTarget);

    UMTD_HealthComponent *HealthComponent = Enemy->GetHealthComponent();
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnPawnDied);

    Enemy->OnStartAttackingDelegate.AddDynamic(this, &ThisClass::OnStartAttacking);
    Enemy->OnStopAttackingDelegate.AddDynamic(this, &ThisClass::OnStopAttacking);

    GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::PostOnPossess);

    OwnerMovementComponent = FMTD_Utility::GetActorComponent<UMovementComponent>(InPawn);
    if (!IsValid(OwnerMovementComponent))
    {
        MTDS_WARN("Owner [%s] does not have a Movement Component.", *InPawn->GetName());
        return;
    }
}

void AMTD_EnemyController::PostOnPossess()
{
    auto Enemy = Cast<AMTD_BaseEnemyCharacter>(GetPawn());
    
    StartRunningBehaviorTree(Enemy);
    SetupKnockbacks(Enemy);
}

void AMTD_EnemyController::OnPawnDied_Implementation(AActor *Actor)
{
    BehaviorTreeComponent->StopTree();
}

void AMTD_EnemyController::OnNewTarget(AActor *OldTarget, AActor *NewTarget)
{
    if (IsValid(OldTarget))
    {
        auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(OldTarget);
        check(HealthComponent);
        HealthComponent->OnDeathStarted.RemoveDynamic(this, &ThisClass::OnTargetDied);
    }
    
    if (IsValid(NewTarget))
    {
        auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(NewTarget);
        check(HealthComponent);
        HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnTargetDied);
    }

    GetBlackboardComponent()->SetValueAsObject(FName("Target"), NewTarget);
}

void AMTD_EnemyController::OnTargetDied(AActor *Actor)
{
    GetBlackboardComponent()->SetValueAsObject(FName("Target"), nullptr);
}

void AMTD_EnemyController::OnStartAttacking()
{
    if (!bAttack)
    {
        bAttack = true;
        GetBlackboardComponent()->SetValueAsBool(FName("Attack"), bAttack);
    }
}

void AMTD_EnemyController::OnStopAttacking()
{
    if (bAttack)
    {
        bAttack = false;
        GetBlackboardComponent()->SetValueAsBool(FName("Attack"), bAttack);
    }
}

void AMTD_EnemyController::OnKnockback(const UMTD_BalanceHitData *HitData)
{
    check(HitData);

    if (KnockbackTimerHandle.IsValid())
    {
        return;
    }

    GetBlackboardComponent()->SetValueAsBool(FName("Knockback"), true);
    GetWorldTimerManager().SetTimer(KnockbackTimerHandle, this, &ThisClass::ResetKnockback, 0.01f, false);
}

void AMTD_EnemyController::ResetKnockback()
{
    KnockbackTimerHandle.Invalidate();
    GetBlackboardComponent()->SetValueAsBool(FName("Knockback"), false);
}

void AMTD_EnemyController::StartRunningBehaviorTree(AMTD_BaseEnemyCharacter *Enemy)
{
    UBehaviorTree *BehaviorTree = Enemy->GetBehaviorTree();
    if (!IsValid(BehaviorTree))
    {
        MTDS_WARN("Enemy [%s] does not have a Behavior Tree.", *Enemy->GetName());
        return;
    }

    GetBlackboardComponent()->InitializeBlackboard(*(BehaviorTree->BlackboardAsset));
    BehaviorTreeComponent->StartTree(*BehaviorTree, EBTExecutionMode::Looped);
}

void AMTD_EnemyController::SetupKnockbacks(AMTD_BaseEnemyCharacter *Enemy)
{
    UMTD_BalanceComponent *BalanceComponent = Enemy->GetBalanceComponent();
    BalanceComponent->OnBalanceDownDelegate.AddDynamic(this, &ThisClass::OnKnockback);

    const auto EnemyExtensionComponent = UMTD_EnemyExtensionComponent::FindEnemyExtensionComponent(Enemy);
    const auto EnemyData = EnemyExtensionComponent->GetEnemyData<UMTD_EnemyData>();
    if (IsValid(EnemyData))
    {
        GetBlackboardComponent()->SetValueAsFloat(FName("KnockbackTime"), EnemyData->KnockbackTime);
    }
}
