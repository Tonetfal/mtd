#include "Player/MTD_FoeController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Components/MTD_BalanceComponent.h"
#include "Character/MTD_BaseFoeCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/Components/MTD_HealthComponent.h"
#include "Character/Components/MTD_TeamComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/MTD_Utility.h"

AMTD_FoeController::AMTD_FoeController()
{
    // Ticking is required to determine controller rotation
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("Behavior Tree Component");
    Blackboard = CreateDefaultSubobject<UBlackboardComponent>("Blackboard Component");
    Team = CreateDefaultSubobject<UMTD_TeamComponent>("MTD Team Component");

    // Follow foe so others can see our team
    bAttachToPawn = true;
    
    // Foe makes use of gameplay ability system, so PS is required since it contains it
    bWantsPlayerState = true;
}

void AMTD_FoeController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    auto OwningCharacter = CastChecked<AMTD_BaseFoeCharacter>(InPawn);

    // Listen for new target event
    OwningCharacter->OnNewTargetDelegate.AddDynamic(this, &ThisClass::OnNewTarget);

    UMTD_HealthComponent *HealthComponent = OwningCharacter->GetHealthComponent();
    check(IsValid(HealthComponent));
    
    // Listen for death event
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnPawnDied);

    // Listen for start and stop attack events
    OwningCharacter->OnStartAttackingDelegate.AddDynamic(this, &ThisClass::OnStartAttacking);
    OwningCharacter->OnStopAttackingDelegate.AddDynamic(this, &ThisClass::OnStopAttacking);

    // Call PostOnPosses next tick
    GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::PostOnPossess);

    // Cache movement component
    OwnerMovementComponent = InPawn->GetMovementComponent();
    if (!IsValid(OwnerMovementComponent))
    {
        MTDS_WARN("Owner [%s] does not have a movement component.", *InPawn->GetName());
        return;
    }
}

void AMTD_FoeController::PostOnPossess()
{
    const auto OwningCharacter = CastChecked<AMTD_BaseFoeCharacter>(GetPawn());

    // Finish initialization
    StartRunningBehaviorTree(OwningCharacter);
    SetupKnockBacks(OwningCharacter);
}

void AMTD_FoeController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const AActor *FocusActor = GetFocusActor();
    if (((!IsValid(FocusActor)) && (IsValid(OwnerMovementComponent))))
    {
        // Set controller rotation to current velocity rotation
        const FRotator VelocityRotation = OwnerMovementComponent->Velocity.ToOrientationRotator();
        SetControlRotation(VelocityRotation);
    }
}

void AMTD_FoeController::OnPawnDied_Implementation(AActor *Actor)
{
    // Stop behavior when dying
    BehaviorTreeComponent->StopTree();
}

void AMTD_FoeController::OnNewTarget(AActor *OldTarget, AActor *NewTarget)
{
    if (IsValid(OldTarget))
    {
        auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(OldTarget);
        check(IsValid(HealthComponent));
        
        // Avoid listening for death event on old target
        HealthComponent->OnDeathStarted.RemoveDynamic(this, &ThisClass::OnTargetDied);
    }
    
    if (IsValid(NewTarget))
    {
        auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(NewTarget);
        check(IsValid(HealthComponent));
        
        // Listen for death event on new target
        HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnTargetDied);
    }

    // Notify blackboard about the new target
    GetBlackboardComponent()->SetValueAsObject(FName("Target"), NewTarget);
}

void AMTD_FoeController::OnTargetDied(AActor *Actor)
{
    // Nullify target value on blackboard
    GetBlackboardComponent()->SetValueAsObject(FName("Target"), nullptr);
}

void AMTD_FoeController::OnStartAttacking()
{
    if (!bAttack)
    {
        // Save local attack state
        bAttack = true;

        // Notify blackboard about attacking new state
        GetBlackboardComponent()->SetValueAsBool(FName("Attack"), bAttack);
    }
}

void AMTD_FoeController::OnStopAttacking()
{
    if (bAttack)
    {
        // Save local attack state
        bAttack = false;
        
        // Notify blackboard about attacking new state
        GetBlackboardComponent()->SetValueAsBool(FName("Attack"), bAttack);
    }
}

void AMTD_FoeController::OnKnockBack(const UMTD_BalanceHitData *HitData)
{
    check(IsValid(HitData));

    // Avoid setting the blackboard if it's already set to true
    if (!KnockBackTimerHandle.IsValid())
    {
        // Notify blackboad about new knockback state
        GetBlackboardComponent()->SetValueAsBool(FName("Knockback"), true);
    }

    // Call a reset function as soon as knockback finishes, or reset the timer if already set
    GetWorldTimerManager().SetTimer(
        KnockBackTimerHandle, this, &ThisClass::ResetKnockBack, FoeData->KnockbackTime, false);
}

void AMTD_FoeController::ResetKnockBack()
{
    // Invalidate the handle to show that timer is not running anymore
    KnockBackTimerHandle.Invalidate();
    
    // Notify blackboad about new knockback state
    GetBlackboardComponent()->SetValueAsBool(FName("Knockback"), false);
}

void AMTD_FoeController::StartRunningBehaviorTree(const AMTD_BaseFoeCharacter *OwningCharacter)
{
    check(IsValid(OwningCharacter));

    UBehaviorTree *BehaviorTree = OwningCharacter->GetBehaviorTree();
    if (!IsValid(BehaviorTree))
    {
        MTDS_WARN("Foe [%s] does not have a behavior tree.", *OwningCharacter->GetName());
        return;
    }

    // Initialize and start the AI
    GetBlackboardComponent()->InitializeBlackboard(*(BehaviorTree->BlackboardAsset));
    BehaviorTreeComponent->StartTree(*BehaviorTree, EBTExecutionMode::Looped);
}

void AMTD_FoeController::SetupKnockBacks(const AMTD_BaseFoeCharacter *OwningCharacter)
{
    check(IsValid(OwningCharacter));

    UMTD_BalanceComponent *BalanceComponent = OwningCharacter->GetBalanceComponent();
    check(IsValid(BalanceComponent));

    // Listen for knockback event
    BalanceComponent->OnBalanceDownDelegate.AddDynamic(this, &ThisClass::OnKnockBack);

    // Cache foe data
    FoeData = OwningCharacter->GetFoeData();
    if (IsValid(FoeData))
    {
        // Set knockback time on blackboard to know how long a knockback lasts for
        GetBlackboardComponent()->SetValueAsFloat(FName("KnockbackTime"), FoeData->KnockbackTime);
    }
}
