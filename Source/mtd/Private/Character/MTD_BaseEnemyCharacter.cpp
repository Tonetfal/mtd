#include "Character/MTD_BaseEnemyCharacter.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "AbilitySystemComponent.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_EnemyExtensionComponent.h"
#include "Character/MTD_HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Equipment/MTD_EquipmentManagerComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/MTD_GameModeBase.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Utility/MTD_Utility.h"

AMTD_BaseEnemyCharacter::AMTD_BaseEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    UCapsuleComponent *CollisionComponent = GetCapsuleComponent();
    CollisionComponent->SetCollisionProfileName(EnemyCollisionProfileName);

    SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sight Sphere"));
    SightSphere->SetupAttachment(GetRootComponent());

    SightSphere->InitSphereRadius(100.f);
    SightSphere->SetCollisionProfileName(SightSphereCollisionProfileName);
    SightSphere->SetGenerateOverlapEvents(true);
    SightSphere->ShapeColor = FColor::Green;

    LoseSightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Lose Sight Sphere"));
    LoseSightSphere->SetupAttachment(SightSphere);

    LoseSightSphere->InitSphereRadius(150.f);
    LoseSightSphere->SetCollisionProfileName(SightSphereCollisionProfileName);
    LoseSightSphere->SetGenerateOverlapEvents(true);
    LoseSightSphere->ShapeColor = FColor::Red;

    AttackTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Trigger"));
    AttackTrigger->SetupAttachment(GetRootComponent());

    AttackTrigger->InitBoxExtent(FVector(20.f, 20.f, 85.f));
    AttackTrigger->SetCollisionProfileName(EnemyAttackCollisionProfileName);
    AttackTrigger->SetGenerateOverlapEvents(true);

    EnemyExtensionComponent = CreateDefaultSubobject<UMTD_EnemyExtensionComponent>(TEXT("Enemy Extension Component"));

    GetHealthComponent()->OnHealthChangedDelegate.AddDynamic(this, &ThisClass::OnHealthChanged);
    
    SightSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSightSphereBeginOverlap);
    LoseSightSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnLoseSightSphereEndOverlap);

    AttackTrigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackTriggerBeginOverlap);
    AttackTrigger->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnAttackTriggerEndOverlap);
}

void AMTD_BaseEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitializeAttributes();
    EquipDefaultWeapon();
}

void AMTD_BaseEnemyCharacter::InitializeAttributes()
{
    const auto EnemyData = EnemyExtensionComponent->GetEnemyData<UMTD_EnemyData>();
    if (!IsValid(EnemyData))
    {
        MTDS_WARN("Enemy Data on Enemy [%s] is invalid.", *GetName());
        return;
    }

    if (!IsValid(EnemyData->TemporaryAttributeTable))
    {
        MTDS_WARN("Attribute Table on Owner [%s]'s Enemy Data is invalid.", *GetName());
        return;
    }

    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    if (!IsValid(Asc))
    {
        MTDS_WARN("Ability System Component on Enemy [%s] is invalid.", *GetName());
        return;
    }

    float Value;
    float TemporaryLevel = 1.f;

    EVALUTE_ATTRIBUTE(EnemyData->TemporaryAttributeTable, HealthScaleAttributeName, TemporaryLevel, Value);
    Value *= EnemyData->Health;
    Asc->ApplyModToAttribute(UMTD_HealthSet::GetMaxHealthAttribute(), EGameplayModOp::Type::Override, Value);
    Asc->ApplyModToAttribute(UMTD_HealthSet::GetHealthAttribute(), EGameplayModOp::Type::Override, Value);

    Value = EnemyData->Mana;
    Asc->ApplyModToAttribute(UMTD_ManaSet::GetMaxManaAttribute(), EGameplayModOp::Type::Override, Value);
    Asc->ApplyModToAttribute(UMTD_ManaSet::GetManaAttribute(), EGameplayModOp::Type::Override, Value);

    EVALUTE_ATTRIBUTE(EnemyData->TemporaryAttributeTable, DamageScaleScaleAttributeName, TemporaryLevel, Value);
    Value *= EnemyData->Damage;
    Asc->ApplyModToAttribute(UMTD_CombatSet::GetDamageBaseAttribute(), EGameplayModOp::Type::Override, Value);

    // EVALUTE_ATTRIBUTE(EnemyData->TemporaryAttributeTable, SpeedScaleScaleAttributeName, TemporaryLevel, Value);
    // Value *= EnemyData->Speed;
    // ...

    Asc->ApplyModToAttribute(UMTD_BalanceSet::GetBaseDamageAttribute(), EGameplayModOp::Type::Override,
        EnemyData->BalanceDamage);
    Asc->ApplyModToAttribute(UMTD_BalanceSet::GetThresholdAttribute(), EGameplayModOp::Type::Override,
        EnemyData->BalanceThreshold);
    Asc->ApplyModToAttribute(UMTD_BalanceSet::GetResistAttribute(), EGameplayModOp::Type::Override,
        EnemyData->BalanceResist);

    MTDS_VERBOSE("Enemy [%s]'s attributes have been initialized.", *GetName());
}

void AMTD_BaseEnemyCharacter::OnDeathStarted_Implementation(AActor *OwningActor)
{
    Super::OnDeathStarted_Implementation(OwningActor);

    DetachFromControllerPendingDestroy();
    DisableCollisions();
}

void AMTD_BaseEnemyCharacter::EquipDefaultWeapon()
{
    if (!IsValid(DefaultWeaponDefinitionClass))
    {
        MTDS_WARN("Default Weapon Definition Class is not set. Enemy [%s] will have no weapon.", *GetName());
        return;
    }

    UMTD_EquipmentManagerComponent *EquipManager = GetEquipmentManagerComponent();
    EquipManager->EquipItem(DefaultWeaponDefinitionClass);
}

void AMTD_BaseEnemyCharacter::SetNewTarget(APawn *Pawn)
{
    if (Pawn == Target)
    {
        return;
    }

    APawn *OldTarget = Target;
    APawn *NewTarget = Pawn;

    if (IsValid(OldTarget))
    {
        auto OldHealthComponent = UMTD_HealthComponent::FindHealthComponent(OldTarget);
        check(OldHealthComponent);
        OldHealthComponent->OnDeathStarted.RemoveDynamic(this, &ThisClass::OnTargetDied);
    }

    if (IsValid(NewTarget))
    {
        auto NewHealthComponent = UMTD_HealthComponent::FindHealthComponent(NewTarget);
        check(NewHealthComponent);
        NewHealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnTargetDied);
    }

    Target = NewTarget;
    OnNewTargetDelegate.Broadcast(OldTarget, NewTarget);
}

APawn *AMTD_BaseEnemyCharacter::GetClosestTarget()
{
    const FMTD_PathFindingContext Context = FMTD_PathFindingContext::Create(this);
    if (!Context.IsValid())
    {
        MTDS_WARN("Path Finding Context on owner [%s] couldn't be created correctly.", *GetName());
        return nullptr;
    }

    // Store final result in here
    float LowestCost = 0.f;
    APawn *Result = nullptr;

    for (APawn *Pawn : DetectedTargets)
    {
        float Cost = 0.f;
        const ENavigationQueryResult::Type QueryResult = FMTD_Utility::ComputePathTo(Pawn, Cost, Context);

        // If the target is reachable check whether its the most optimal path cost wise
        if (QueryResult == ENavigationQueryResult::Success)
        {
            if ((!Result) || (LowestCost > Cost))
            {
                LowestCost = Cost;
                Result = Pawn;
            }
        }
    }

    return Result;
}

bool AMTD_BaseEnemyCharacter::IsActorInRedirectRange(const APawn *Pawn) const
{
    const FVector A = GetNavAgentLocation();
    const FVector B = Pawn->GetNavAgentLocation();
    const FVector Disp = B - A;
    const float Len = Disp.Length();

    return (Len <= RetargetMaxRange);
}

void AMTD_BaseEnemyCharacter::OnTargetDied(AActor *Actor)
{
    SetNewTarget(nullptr);
}

AActor *AMTD_BaseEnemyCharacter::GetCheapiestActor(AActor *Lhs, AActor *Rhs) const
{
    const FMTD_PathFindingContext Context = FMTD_PathFindingContext::Create(this);
    if (!Context.IsValid())
    {
        MTDS_WARN("Path Finding Context on owner [%s] couldn't be created correctly.", *GetName());
        return nullptr;
    }

    float CostToLhs = 0.f;
    float CostToRhs = 0.f;

    ENavigationQueryResult::Type QueryResult;
    QueryResult = FMTD_Utility::ComputePathTo(Lhs, CostToLhs, Context);

    // If Rhs is not reacheable return Lhs regardless; it may be unreacheable as well
    if (QueryResult != ENavigationQueryResult::Success)
    {
        return Lhs;
    }

    QueryResult = FMTD_Utility::ComputePathTo(Rhs, CostToRhs, Context);

    // At this point we know that Rhs is reacheable, hence if Lhs isn't so, return Lhs
    if (QueryResult != ENavigationQueryResult::Success)
    {
        return Rhs;
    }

    // Return actor that's the cheapiest to get to
    return (CostToLhs > CostToRhs) ? (Rhs) : (Lhs);
}

void AMTD_BaseEnemyCharacter::OnHealthChanged_Implementation(UMTD_HealthComponent *InHealthComponent, float OldValue,
    float NewValue, AActor *InInstigator)
{
    const float Damage = OldValue - NewValue;
    if (Damage <= 0.f)
    {
        return;
    }

    auto Ps = CastChecked<APlayerState>(InInstigator);
    APawn *InstigatorPawn = Ps->GetPawn();

    // A pawn may die in case of delay damage, while the PS will be still instantiated
    if (!IsValid(InstigatorPawn))
    {
        return;
    }

    // Avoid trying to change target if it's the same
    if (InstigatorPawn == Target)
    {
        return;
    }

    // Avoid players
    // TODO: They should only be targetted if inside sight sphere
    if (InstigatorPawn->IsA(AMTD_BasePlayerCharacter::StaticClass()))
    {
        return;
    }

    // Don't run following heavy code too often, hence there is a simple lock
    if (bRetargetLock)
    {
        return;
    }

    bRetargetLock = true;
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::UnlockRetarget, TimeToUnlockRetarget, false);

    if (!IsActorInRedirectRange(InstigatorPawn))
    {
        return;
    }

    APawn *FinalTarget = InstigatorPawn;

    // Find out what's the cheapiest actor to get to among the current one and the damage dealer
    if (IsValid(Target))
    {
        FinalTarget = Cast<APawn>(GetCheapiestActor(Target, InstigatorPawn));
    }

    // In case that we're following the game target, find out what's the cheapiest actor to follow between it and the
    // damage dealer
    else
    {
        const auto GameMode = CastChecked<AMTD_GameModeBase>(GetWorld()->GetAuthGameMode());
        AActor *GameTarget = GameMode->GetGameTarget(this);

        // Should always be the case
        if (IsValid(GameTarget))
        {
            const AActor *CheapiestActor = nullptr;
            CheapiestActor = GetCheapiestActor(Target, GameTarget);

            // GameTarget is implicit; if there is no target, the AI will go towards GameTarget
            FinalTarget = (CheapiestActor == GameTarget) ? (nullptr) : (Target);
        }
    }

    SetNewTarget(FinalTarget);
}

void AMTD_BaseEnemyCharacter::OnDeathFinished_Implementation(AActor *OwningActor)
{
    // Don't call the default implementation

    GetEquipmentManagerComponent()->UnequipItem();
}

void AMTD_BaseEnemyCharacter::OnGameTerminated_Implementation(EMTD_GameResult GameResult)
{
    Super::OnGameTerminated_Implementation(GameResult);
    DetachFromControllerPendingDestroy();
}

void AMTD_BaseEnemyCharacter::DisableCollisions()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SightSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LoseSightSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AttackTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMTD_BaseEnemyCharacter::OnSightSphereBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    auto Pawn = CastChecked<APawn>(OtherActor);
    DetectedTargets.Add(Pawn);

    if (!IsValid(Target))
    {
        SetNewTarget(Pawn);
    }
}

void AMTD_BaseEnemyCharacter::OnLoseSightSphereEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    auto Pawn = CastChecked<APawn>(OtherActor);
    DetectedTargets.Remove(Pawn);

    if (OtherActor == Target)
    {
        APawn *NewTarget = GetClosestTarget();
        SetNewTarget(NewTarget);
    }
}

void AMTD_BaseEnemyCharacter::OnAttackTriggerBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    auto Pawn = CastChecked<APawn>(OtherActor);
    const int32 Index = AttackTargets.Add(Pawn);

    if (Index == 0)
    {
        OnStartAttackingDelegate.Broadcast();
    }
}

void AMTD_BaseEnemyCharacter::OnAttackTriggerEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    auto Pawn = CastChecked<APawn>(OtherActor);
    AttackTargets.Remove(Pawn);

    if (AttackTargets.IsEmpty())
    {
        OnStopAttackingDelegate.Broadcast();
    }
}
