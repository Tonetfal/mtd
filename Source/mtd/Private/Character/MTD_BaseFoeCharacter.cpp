#include "Character/MTD_BaseFoeCharacter.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystemComponent.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_HealthComponent.h"
#include "CombatSystem/MTD_CombatComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Equipment/MTD_EquipmentManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/MTD_GameModeBase.h"
#include "GameModes/MTD_TowerDefenseMode.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Inventory/MTD_InventoryItemInstance.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MTD_PlayerState.h"
#include "System/MTD_Tags.h"
#include "Utility/MTD_Utility.h"

AMTD_BaseFoeCharacter::AMTD_BaseFoeCharacter()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    UCapsuleComponent *CollisionComponent = GetCapsuleComponent();
    CollisionComponent->SetCollisionProfileName(FoeCollisionProfileName);

    SightSphere = CreateDefaultSubobject<USphereComponent>("Sight Sphere");
    SightSphere->SetupAttachment(GetRootComponent());

    SightSphere->InitSphereRadius(100.f);
    SightSphere->SetCollisionProfileName(SightSphereCollisionProfileName);
    SightSphere->SetGenerateOverlapEvents(true);
    SightSphere->ShapeColor = FColor::Green;

    LoseSightSphere = CreateDefaultSubobject<USphereComponent>("Lose Sight Sphere");
    LoseSightSphere->SetupAttachment(SightSphere);

    LoseSightSphere->InitSphereRadius(150.f);
    LoseSightSphere->SetCollisionProfileName(SightSphereCollisionProfileName);
    LoseSightSphere->SetGenerateOverlapEvents(true);
    LoseSightSphere->ShapeColor = FColor::Red;

    AttackTrigger = CreateDefaultSubobject<UBoxComponent>("Attack Trigger");
    AttackTrigger->SetupAttachment(GetRootComponent());

    AttackTrigger->InitBoxExtent(FVector(20.f, 20.f, 85.f));
    AttackTrigger->SetCollisionProfileName(FoeAttackCollisionProfileName);
    AttackTrigger->SetGenerateOverlapEvents(true);

    // For some reason in PreInitializeComponent this binding triggers ensure macro, hence it's here
    check(IsValid(HealthComponent));
    HealthComponent->OnHealthChangedDelegate.AddDynamic(this, &ThisClass::OnHealthChanged);
    
    check(IsValid(CombatComponent));
    CombatComponent->AddObjectTypeToHit(PlayerQuery);
    CombatComponent->AddObjectTypeToHit(TowerQuery);

    Tags.Add(FMTD_Tags::Foe);
}

void AMTD_BaseFoeCharacter::InitializeAttributes()
{
    if (!IsValid(FoeData))
    {
        MTDS_WARN("Foe data is invalid.");
        return;
    }

    UWorld *World = GetWorld();
    const auto TowerDefenseMode = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(World));
    if (!IsValid(TowerDefenseMode))
    {
        MTDS_WARN("Tower defense mode is invalid.");
        return;
    }

    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability system component is invalid.");
        return;
    }

    // Intermediate variable
    float Value;

    // @todo use an actual formula to compute scaling
    const float Difficulty = TowerDefenseMode->GetScaledDifficulty();
    const auto ScaleValue = [Difficulty] (float Value) { return (Value * Difficulty); };
    
    Value = ScaleValue(FoeData->Health);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_HealthSet::GetMaxHealthAttribute(), Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_HealthSet::GetHealthAttribute(), Value);

    Value = ScaleValue(FoeData->Damage);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_CombatSet::GetDamageBaseAttribute(), Value);

    // @todo implement speed
    // Value = ScaleValue(FoeData->Speed);
    // Asc->SetNumericAttributeBase(UMTD_CombatSet::GetSpeedAttribute(), Value);

    AbilitySystemComponent->SetNumericAttributeBase(UMTD_BalanceSet::GetDamageAttribute(), FoeData->BalanceDamage);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_BalanceSet::GetThresholdAttribute(), FoeData->BalanceThreshold);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_BalanceSet::GetResistAttribute(), FoeData->BalanceResist);

    MTDS_VERBOSE("Attributes have been initialized.");
}

void AMTD_BaseFoeCharacter::PreInitializeComponents()
{
    Super::PreInitializeComponents();
    
    check(IsValid(SightSphere));
    check(IsValid(LoseSightSphere));
    check(IsValid(AttackTrigger));

    // Listen for overlap begin/end events on sight spheres
    SightSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSightSphereBeginOverlap);
    LoseSightSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnLoseSightSphereEndOverlap);

    // Listen for begin/end events overlaps on attack trigger
    AttackTrigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackTriggerBeginOverlap);
    AttackTrigger->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnAttackTriggerEndOverlap);
}

void AMTD_BaseFoeCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize the character
    InitializeAttributes();
    EquipDefaultWeapon();
}

void AMTD_BaseFoeCharacter::OnDeathStarted_Implementation(AActor *OwningActor)
{
    Super::OnDeathStarted_Implementation(OwningActor);

    // Drop EXP/mana/items
    DropGoods();

    // Avoid colliding and moving when dying
    GetCharacterMovement()->DisableMovement();
    DisableCollisions();
}

void AMTD_BaseFoeCharacter::OnDeathFinished_Implementation(AActor *OwningActor)
{
    // Don't call the default implementation

    // Remove controller and player state from world as well as the character
    auto DestroyController = [this] ()
        {
            if (Controller)
            {
                Controller->Destroyed();
            }
        };

    // Delay destruct for next tick because death finish is called from an ability, hence we would destroy an active
    // ability if called just this frame
    GetWorldTimerManager().SetTimerForNextTick(DestroyController);
}

void AMTD_BaseFoeCharacter::OnGameTerminated_Implementation(EMTD_GameResult GameResult)
{
    Super::OnGameTerminated_Implementation(GameResult);
    DetachFromControllerPendingDestroy();
}

void AMTD_BaseFoeCharacter::OnHealthChanged_Implementation(UMTD_HealthComponent *InHealthComponent, float OldValue,
    float NewValue, AActor *InInstigator)
{
    const float DamageDealt = (OldValue - NewValue);
    if (DamageDealt <= 0.f)
    {
        return;
    }

    auto InstigatorPlayerState = CastChecked<APlayerState>(InInstigator);
    APawn *InstigatorPawn = InstigatorPlayerState->GetPawn();

    // A pawn may die in case of delayed damage, while the PS will be still instantiated.
    // For instance a player could fire a projectile us, and die between the fire and landing time
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
            const AActor *CheapiestActor = GetCheapiestActor(InstigatorPawn, GameTarget);

            // GameTarget is implicit; if there is no target, the AI will go towards GameTarget
            FinalTarget = ((CheapiestActor == GameTarget) ? (nullptr) : (Target));
        }
    }

    SetNewTarget(FinalTarget);
}

void AMTD_BaseFoeCharacter::EquipDefaultWeapon()
{
    // Create base inventory item using default item ID
    UMTD_BaseInventoryItemData *ItemData = UMTD_InventoryBlueprintFunctionLibrary::CreateBaseInventoryItemData(
        this, DefaultWeaponID);
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Failed to create a default weapon.");
        return;
    }
    
    AMTD_PlayerState *MtdPlayerState = GetMtdPlayerState();
    if (!IsValid(MtdPlayerState))
    {
        MTDS_WARN("MTD player state is invalid.");
        return;
    }
    
    UMTD_EquipmentManagerComponent *EquipManager = MtdPlayerState->GetEquipmentManagerComponent();
    if (!IsValid(EquipManager))
    {
        MTDS_WARN("Equipment manager component is invalid.");
        return;
    }

    // Equip the created item
    EquipManager->K2_EquipItem(ItemData);
}

void AMTD_BaseFoeCharacter::SetNewTarget(APawn *InTarget)
{
    // Avoid running logic if it's the same pawn
    if (InTarget == Target)
    {
        return;
    }

    APawn *OldTarget = Target;
    APawn *NewTarget = InTarget;

    if (IsValid(OldTarget))
    {
        // Don't listen for death event on old target since we aren't interested in it anymore
        auto OldHealthComponent = UMTD_HealthComponent::FindHealthComponent(OldTarget);
        check(IsValid(OldHealthComponent));
        OldHealthComponent->OnDeathStarted.RemoveDynamic(this, &ThisClass::OnTargetDied);
    }

    if (IsValid(NewTarget))
    {
        // Listen for death event on new target to know to change our behavior
        auto NewHealthComponent = UMTD_HealthComponent::FindHealthComponent(NewTarget);
        check(IsValid(NewHealthComponent));
        NewHealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnTargetDied);
    }

    Target = NewTarget;
    
    // Notify about new target
    OnNewTargetDelegate.Broadcast(OldTarget, NewTarget);
}

APawn *AMTD_BaseFoeCharacter::GetClosestTarget()
{
    const FMTD_PathFindingContext Context = FMTD_PathFindingContext::Create(this);
    if (!Context.IsValid())
    {
        MTDS_WARN("Path finding context could not be created correctly.");
        return nullptr;
    }

    // Store final result in here
    float LowestCost = 0.f;
    APawn *Result = nullptr;

    // Iterate through all detected target, and decide which one is the closest
    for (APawn *Pawn : DetectedTargets)
    {
        float Cost = 0.f;
        const ENavigationQueryResult::Type QueryResult = FMTD_Utility::ComputePathTo(Pawn, Cost, Context);

        // If the target is reachable check whether its the most optimal path cost wise
        if (QueryResult == ENavigationQueryResult::Success)
        {
            if (((!Result) || (LowestCost > Cost)))
            {
                LowestCost = Cost;
                Result = Pawn;
            }
        }
    }

    return Result;
}

AActor *AMTD_BaseFoeCharacter::GetCheapiestActor(AActor *Lhs, AActor *Rhs) const
{
    const FMTD_PathFindingContext Context = FMTD_PathFindingContext::Create(this);
    if (!Context.IsValid())
    {
        MTDS_WARN("Path finding context could not be created correctly.");
        return nullptr;
    }

    float CostToLhs = 0.f;
    float CostToRhs = 0.f;

    // Compute path to lhs
    ENavigationQueryResult::Type QueryResult = FMTD_Utility::ComputePathTo(Lhs, CostToLhs, Context);

    // If Rhs is not reacheable return Lhs regardless; it may be unreacheable as well
    if (QueryResult != ENavigationQueryResult::Success)
    {
        return Lhs;
    }

    // Compute path to rhs
    QueryResult = FMTD_Utility::ComputePathTo(Rhs, CostToRhs, Context);

    // At this point we know that Rhs is reacheable, hence if Lhs isn't so, return Lhs
    if (QueryResult != ENavigationQueryResult::Success)
    {
        return Rhs;
    }

    // Return actor that's the cheapiest to get to
    return ((CostToLhs > CostToRhs) ? (Rhs) : (Lhs));
}

bool AMTD_BaseFoeCharacter::IsActorInRedirectRange(const APawn *Pawn) const
{
    const FVector A = GetNavAgentLocation();
    const FVector B = Pawn->GetNavAgentLocation();
    const FVector Displacement = (B - A);
    const float Length = Displacement.Length();

    return (Length <= RetargetMaxRange);
}

void AMTD_BaseFoeCharacter::OnTargetDied(AActor *InTarget)
{
    // Remove target
    SetNewTarget(nullptr);
}

void AMTD_BaseFoeCharacter::DropGoods_Implementation()
{
    DropItem();
    DropExp();
}

void AMTD_BaseFoeCharacter::DropItem()
{
    const AMTD_InventoryItemInstance *ItemInstance =
        UMTD_InventoryBlueprintFunctionLibrary::GenerateDropItemInstance(this, false);
    
    if (!IsValid(ItemInstance))
    {
        // An item can be not spawned not due an error, but simply due random
        return;
    }
}

void AMTD_BaseFoeCharacter::DropExp()
{
    if (!IsValid(FoeData))
    {
        MTDS_WARN("Foe data is invalid.");
    }
        
    UWorld *World = GetWorld();
    const auto TowerDefenseMode = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(World));
    if (!IsValid(TowerDefenseMode))
    {
        MTDS_WARN("Tower defense mode is invalid.");
        return;
    }
    
    const float Experience = FoeData->Experience;
    const float Difficulty = TowerDefenseMode->GetScaledDifficulty();
    
    // @todo use an actual formula to compute scaling
    const auto ScaleExp = [Difficulty] (float Exp) { return (Exp * Difficulty); };

    // Give experience to all present players
    TowerDefenseMode->BroadcastExp(ScaleExp(Experience));
}

void AMTD_BaseFoeCharacter::DisableCollisions()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SightSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LoseSightSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AttackTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMTD_BaseFoeCharacter::OnSightSphereBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    if (HealthComponent->IsDeadOrDying())
    {
        return;
    }

    // Add to detected targets container
    auto Pawn = CastChecked<APawn>(OtherActor);
    DetectedTargets.Add(Pawn);

    if (!IsValid(Target))
    {
        // If there is no target, target this pawn
        SetNewTarget(Pawn);
    }
}

void AMTD_BaseFoeCharacter::OnLoseSightSphereEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    if (HealthComponent->IsDeadOrDying())
    {
        return;
    }

    // Remove from detected targets container
    auto Pawn = CastChecked<APawn>(OtherActor);
    DetectedTargets.Remove(Pawn);

    if (OtherActor == Target)
    {
        // If it's the current attack target, try to search for a new one
        APawn *NewTarget = GetClosestTarget();
        SetNewTarget(NewTarget);
    }
}

void AMTD_BaseFoeCharacter::OnAttackTriggerBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    if (HealthComponent->IsDeadOrDying())
    {
        return;
    }

    // Add to attack targets container
    auto Pawn = CastChecked<APawn>(OtherActor);
    AttackTargets.Add(Pawn);

    const int32 Num = AttackTargets.Num();
    if (Num == 1)
    {
        // Notify about entering in attack mode if it's the first trigger pawn
        OnStartAttackingDelegate.Broadcast();
    }
}

void AMTD_BaseFoeCharacter::OnAttackTriggerEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    if (HealthComponent->IsDeadOrDying())
    {
        return;
    }

    // Remove from attack targets container
    auto Pawn = CastChecked<APawn>(OtherActor);
    AttackTargets.Remove(Pawn);

    if (AttackTargets.IsEmpty())
    {
        // Notify about leaving attack mdoe if it was the last trigger pawn
        OnStopAttackingDelegate.Broadcast();
    }
}
