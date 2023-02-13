#include "Character/MTD_Tower.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_HealthComponent.h"
#include "Character/MTD_HeroComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameModes/MTD_GameModeBase.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Player/MTD_PlayerState.h"
#include "Player/MTD_TowerController.h"
#include "Projectile/MTD_Projectile.h"
#include "Projectile/MTD_ProjectileMovementComponent.h"
#include "System/MTD_Tags.h"

AMTD_Tower::AMTD_Tower()
{
    // Tower should tick, but only after told so
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    // Use MTD_TowerController as the default AI controller class
    AIControllerClass = AMTD_TowerController::StaticClass();

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>("Collision Component");
    SetRootComponent(CollisionComponent);
    CollisionComponent->SetCollisionProfileName(TowerCollisionProfileName);
    CollisionComponent->SetSimulatePhysics(false);
    CollisionComponent->SetEnableGravity(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    NavVolumeComponent = CreateDefaultSubobject<UBoxComponent>("Navigation Volume Component");
    NavVolumeComponent->SetupAttachment(GetRootComponent());
    NavVolumeComponent->SetCollisionProfileName("NoCollision");
    NavVolumeComponent->SetSimulatePhysics(false);
    NavVolumeComponent->SetEnableGravity(false);
    NavVolumeComponent->SetCanEverAffectNavigation(true);
    NavVolumeComponent->bDynamicObstacle = true;

    ProjectileSpawnPosition = CreateDefaultSubobject<USphereComponent>("Projectile Spawn Position");
    ProjectileSpawnPosition->SetupAttachment(GetRootComponent());
    ProjectileSpawnPosition->SetCollisionProfileName("NoCollision");
    ProjectileSpawnPosition->SetSimulatePhysics(false);
    ProjectileSpawnPosition->SetEnableGravity(false);
    ProjectileSpawnPosition->SetCanEverAffectNavigation(false);

    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh Component");
    MeshComponent->SetupAttachment(GetRootComponent());
    MeshComponent->SetCollisionProfileName("NoCollision");
    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetEnableGravity(false);
    MeshComponent->SetCanEverAffectNavigation(false);

    PawnExtentionComponent = CreateDefaultSubobject<UMTD_PawnExtensionComponent>("MTD Pawn Extension Component");
    PawnExtentionComponent->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
    PawnExtentionComponent->OnAbilitySystemUninitialized_Register(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

    HeroComponent = CreateDefaultSubobject<UMTD_HeroComponent>("MTD Hero Component");
    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>("MTD Health Component");
    
    // Listen for death events. For some reason if these are added outside the constructor the delegates are added
    // twice, hence they're added in here instead
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

    bCanAffectNavigationGeneration = false;

    Tags.Add(FMTD_Tags::Tower);
}

void AMTD_Tower::InitializeAttributes()
{
    // @todo: think how to pass get the scaled range instead
    // Dispatch the Attribute Table regardless the ASC presence due SBS. The SBS may spawn a tower without a controller,
    // hence the ASC will not be initialized. However, the reason SBS may spawn it is because it needs to retrieve
    // vision related data along the owner player stats. The spawned tower will have PlayerCharacter as Owner and its
    // PlayerState as Instigator.

    ReadTableBaseValues(CurrentLevel);

    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        // Avoid logging due the reason written above
        return;
    }

    AbilitySystemComponent->SetNumericAttributeBase(UMTD_HealthSet::GetMaxHealthAttribute(), BaseHealth);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_BalanceSet::GetDamageAttribute(), BalanceDamage);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_BalanceSet::GetResistAttribute(), 100.f);

    // Apply health scaling after max health has been set
    ApplyTowerHealthScaling();

    // Notify about range attribute change
    OnRangeAttributeChangedDelegate.Broadcast();
    
    MTDS_VERBOSE("Attributes have been initialized.");
}

void AMTD_Tower::ReadTableBaseValues(int32 InLevel)
{
    ensure(InLevel >= 0);
    ensure(InLevel <= MaxLevel);

    if (!IsValid(TowerData))
    {
        MTDS_WARN("Tower data is invalid.");
        return;
    }

    // Intermediate variable
    float Value;

    // Read each value from attribute table using the given level
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, DamageAttributeName, InLevel, Value);
    BaseDamage = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, RangeAttributeName, InLevel, Value);
    BaseVisionRange = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, VisionDegreesAttributeName, InLevel, Value);
    BaseVisionHalfDegrees = (Value / 2.f);
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, FirerateAttributeName, InLevel, Value);
    BaseFirerate = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, ProjectileSpeedAttributeName, InLevel, Value);
    BaseProjectileSpeed = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, BalanceDamageAttributeName, InLevel, Value);
    BalanceDamage = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, HealthAttributeName, InLevel, Value);
    BaseHealth = Value;
}

void AMTD_Tower::ApplyTowerHealthScaling()
{
    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability system component is invalid.");
        return;
    }
    
    if (!IsValid(InstigatorAbilitySystemComponent))
    {
        MTDS_WARN("Instigator ability system component is invalid.");
        return;
    }

    if (!TowerHealthAttributeScalingGeClass)
    {
        MTDS_WARN("Tower health attribute scaling gameplay effect class is NULL.");
        return;
    }

    // Create context to pass tower actor
    FGameplayEffectContextHandle EffectContext = InstigatorAbilitySystemComponent->MakeEffectContext();
    EffectContext.AddActors({ this }, true);

    if (!EffectContext.IsValid())
    {
        MTDS_WARN("Failed to create an effect context.");
        return;
    }

    // Create GE spec with the created context
    const FGameplayEffectSpecHandle &Spec = InstigatorAbilitySystemComponent->MakeOutgoingSpec(
        TowerHealthAttributeScalingGeClass, 1.f, EffectContext);

    if (!Spec.IsValid())
    {
        MTDS_WARN("Failed to create an outgoing spec for tower health attribute scaling gameplay effect.");
        return;
    }
    
    // Apply max health scaling
    InstigatorAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, AbilitySystemComponent);

    // Spec will be applied later on, hence delay the heal
    GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::MaxHealthRestore));
}

void AMTD_Tower::MaxHealthRestore()
{
    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        return;
    }
    
    // Max health may change, hence get the value to heal the tower up
    const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UMTD_HealthSet::GetMaxHealthAttribute());

    // Start with full health
    AbilitySystemComponent->ApplyModToAttribute(
        UMTD_HealthSet::GetHealthAttribute(), EGameplayModOp::Type::Override, MaxHealth);
}

void AMTD_Tower::BeginPlay()
{
    Super::BeginPlay();

    // Run all initialization logic
    CacheInstigatorAbilitySystemComponent();
    ListenForGameTerminated();
    ListenForRangeAttributeChanges();

    if (!CheckTowerDataValidness())
    {
        // Avoid enabling ticking, which will run all main tower logic
        return;
    }
    
    InitializeAttributes();
    
    // All initialization has run successfully: Enable ticking, which is disabled by default
    SetActorTickEnabled(true);
}

void AMTD_Tower::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsReloading)
    {
        // We're on reload
        return;
    }
    
    if (!IsValid(TowerController))
    {
        MTDS_WARN("Controller is invalid. Disabling ticking.");
        SetActorTickEnabled(false);
        return;
    }

    // Get a target to fire at
    AActor *FireTarget = TowerController->GetFireTarget();
    if (!IsValid(FireTarget))
    {
        return;
    }

    // Fire projectile at that target
    FireProjectile(FireTarget);
}

void AMTD_Tower::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    if (IsValid(Controller))
    {
        // Save controller as MTD tower controller to avoid casting base controller many times
        TowerController = CastChecked<AMTD_TowerController>(Controller);
    }

    // Notify pawn extension component about controller change
    PawnExtentionComponent->HandleControllerChanged();
}

int32 AMTD_Tower::GetCurrentLevel() const
{
    return CurrentLevel;
}

void AMTD_Tower::AddLevel(int32 InDeltaLevel)
{
    if (InDeltaLevel < 1)
    {
        MTDS_WARN("Delta level [%d] must be a positive number.", InDeltaLevel);
        return;
    }
    
    if (CurrentLevel == MaxLevel)
    {
        MTDS_WARN("Cannot add any level because current level [%d] is the maximum one.", CurrentLevel);
        return;
    }

    // Clamp value if required
    const int32 MaxDeltaLevel = (MaxLevel - CurrentLevel);
    const int32 DeltaLevel = FMath::Min(InDeltaLevel, MaxDeltaLevel);
    if (InDeltaLevel != DeltaLevel)
    {
        MTDS_WARN("Delta level [%d] has been decreased to [%d].", InDeltaLevel, DeltaLevel);
    }

    // Perform main level up logic
    const int32 OldLevel = CurrentLevel;
    CurrentLevel = (CurrentLevel + DeltaLevel);
    OnLevelUp(CurrentLevel, OldLevel);

    // Send gameplay event
    SendLevelUpEvent();

    // Notify about level up
    OnLevelUpDelegate.Broadcast(CurrentLevel, OldLevel);
}

void AMTD_Tower::OnGameTerminated_Implementation(EMTD_GameResult GameResult)
{
    // Avoid running the logic when game terminates
    SetActorTickEnabled(false);
}

AMTD_PlayerState *AMTD_Tower::GetMtdPlayerState() const
{
    return CastChecked<AMTD_PlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UMTD_AbilitySystemComponent *AMTD_Tower::GetMtdAbilitySystemComponent() const
{
    return PawnExtentionComponent->GetMtdAbilitySystemComponent();
}

UAbilitySystemComponent *AMTD_Tower::GetAbilitySystemComponent() const
{
    return GetMtdAbilitySystemComponent();
}

float AMTD_Tower::GetScaledDamage_Implementation() const
{
    float Scale = 1.f;
    if (IsValid(InstigatorBuilderSet))
    {
        // Evaluate scaling given damage total stat attribute
        const float DamageStat = InstigatorBuilderSet->GetDamageStat();
        const float DamageStatBonus = InstigatorBuilderSet->GetDamageStat_Bonus();
        const float TotalDamageStat = (DamageStat + DamageStatBonus);
        
        auto Formula = [] (float T)
            {
                T = FMath::Max(T, 0.f); // Avoid negative values
                return (1.f + (T / 100.f));
            };
        Scale = Formula(TotalDamageStat);
    }
    
    return (BaseDamage * Scale);
}

float AMTD_Tower::GetScaledFirerate_Implementation() const
{
    float Scale = 1.f;
    if (IsValid(InstigatorBuilderSet))
    {
        // Evaluate scaling given speed total stat attribute
        const float SpeedStat = InstigatorBuilderSet->GetSpeedStat();
        const float SpeedStatBonus = InstigatorBuilderSet->GetSpeedStat_Bonus();
        const float TotalSpeedStat = (SpeedStat + SpeedStatBonus);
        
        auto Formula = [] (float T)
            {
                T = FMath::Max(T, 0.f); // Avoid negative values
                return (1.f + (T / 100.f));
            };
        Scale = Formula(TotalSpeedStat);
    }
    
    return (BaseFirerate * Scale);
}

float AMTD_Tower::GetScaledVisionRange_Implementation() const
{
    float Scale = 1.f;
    if (IsValid(InstigatorBuilderSet))
    {
        // Evaluate scaling given range total stat attribute
        const float RangeStat = InstigatorBuilderSet->GetRangeStat();
        const float RangeStatBonus = InstigatorBuilderSet->GetRangeStat_Bonus();
        const float TotalRangeStat = (RangeStat + RangeStatBonus);
        
        auto Formula = [] (float T)
            {
                T = FMath::Max(T, 0.f); // Avoid negative values
                return (1.f + (T / 100.f));
            };
        Scale = Formula(TotalRangeStat);
    }
    
    return (BaseVisionRange * Scale);
}

float AMTD_Tower::GetScaledVisionHalfDegrees_Implementation() const
{
    return BaseVisionHalfDegrees;
}

float AMTD_Tower::GetScaledProjectileSpeed_Implementation() const
{
    float Scale = 1.f;
    if (IsValid(InstigatorBuilderSet))
    {
        // Evaluate scaling given speed total stat attribute
        const float SpeedStat = InstigatorBuilderSet->GetSpeedStat();
        const float SpeedStatBonus = InstigatorBuilderSet->GetSpeedStat_Bonus();
        const float TotalSpeedStat = (SpeedStat + SpeedStatBonus);
        
        auto Formula = [] (float T)
            {
                T = FMath::Max(T, 0.f); // Avoid negative values
                return (1.f + (T / 100.f));
            };
        Scale = Formula(TotalSpeedStat);
    }
    
    return (BaseProjectileSpeed * Scale);
}

float AMTD_Tower::GetReloadTime_Implementation() const
{
    // Fire 'BaseFirerate' times per second, i.e. if firerate is 10, then reload time will be 6 seconds,
    // hence maximum 10 projectiles will be fired
    const float Firerate = GetScaledFirerate();
    return ((Firerate == 0.f) ? (60.f) : (60.f / Firerate));
}

void AMTD_Tower::OnLevelUp(int32 NewLevel, int32 OldLevel)
{
    // Read attribute table with the new level
    ReadTableBaseValues(NewLevel);

    // Heal tower up on level up
    MaxHealthRestore();
}

void AMTD_Tower::SendLevelUpEvent()
{
    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        return;
    }

    const AActor *Avatar = AbilitySystemComponent->GetAvatarActor();
    if (!IsValid(Avatar))
    {
        return;
    }

    // Send the "Gameplay.Event.LevelUp" gameplay event through the owner's ability system.
    // This can be used to trigger a level up gameplay ability.
    FGameplayEventData Payload;
    Payload.EventTag = FMTD_GameplayTags::Get().Gameplay_Event_LevelUp;
    Payload.Target = AbilitySystemComponent->GetAvatarActor();
    Payload.ContextHandle = AbilitySystemComponent->MakeEffectContext();;

    // Send a gameplay event
    AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void AMTD_Tower::FireProjectile(AActor *FireTarget)
{
    // Create a projectile
    const FTransform Transform = ProjectileSpawnPosition->GetComponentTransform();
    AMTD_Projectile *Projectile = SpawnProjectile(Transform);
    if (!IsValid(Projectile))
    {
        return;
    }

    // Setup and finilize the projectile
    SetupProjectile(*Projectile, FireTarget);
    Projectile->FinishSpawning(Transform, true);

    // Start reloading after a successful fire
    StartReloading();
}

void AMTD_Tower::StartReloading()
{
    ensure(!bIsReloading);

    // Change state
    bIsReloading = true;

    // Setup an event to notify about reload finish
    const float ReloadTime = GetReloadTime();
    GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ThisClass::OnReloadFinished, ReloadTime, false);
}

void AMTD_Tower::OnReloadFinished()
{
    ensure(bIsReloading);

    // Change state
    bIsReloading = false;
}

void AMTD_Tower::ListenForGameTerminated()
{
    UWorld *World = GetWorld();
    AGameModeBase *GameMode = World->GetAuthGameMode();
    if (IsValid(GameMode))
    {
        auto MtdGameMode = CastChecked<AMTD_GameModeBase>(GameMode);

        // Listen for game terminated event
        MtdGameMode->OnGameTerminatedDelegate.AddDynamic(this, &ThisClass::OnGameTerminated);
    }
}

void AMTD_Tower::ListenForRangeAttributeChanges()
{
    if (!IsValid(InstigatorAbilitySystemComponent))
    {
        MTDS_WARN("Instigator's Ability System Component is invalid.");
        return;
    }

    // Listen for range attribute changes
    InstigatorAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_BuilderSet::GetRangeStatAttribute()).AddUObject(this, &ThisClass::OnRangeAttributeChanged);
    InstigatorAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_BuilderSet::GetRangeStat_BonusAttribute()).AddUObject(this, &ThisClass::OnRangeAttributeChanged);
}

void AMTD_Tower::OnRangeAttributeChanged(const FOnAttributeChangeData &Attribute)
{
    // Notify about range attribute change
    OnRangeAttributeChangedDelegate.Broadcast();
}

void AMTD_Tower::CacheInstigatorAbilitySystemComponent()
{
    const APawn *PawnInstigator = GetInstigator();
    if (!IsValid(PawnInstigator))
    {
        MTDS_WARN("PawnInstigator is invalid");
        return;
    }

    const auto PlayerCharacter = Cast<AMTD_BasePlayerCharacter>(PawnInstigator);
    if (!IsValid(PlayerCharacter))
    {
        MTDS_WARN("Failed to cast PawnInstigator [%s] to MTD Base Player Character.", *PawnInstigator->GetName());
        return;
    }

    // Cache instigator ASC to avoid searching for it every time it's needed
    InstigatorAbilitySystemComponent = PlayerCharacter->GetAbilitySystemComponent();
    if (!IsValid(InstigatorAbilitySystemComponent))
    {
        MTDS_WARN("Ability System Component on PawnInstigator [%s] is invalid.", *PawnInstigator->GetName());
        return;
    }

    // Cache builder set to avoid searching for it every time it's needed
    const UAttributeSet *AttributeSet =
        InstigatorAbilitySystemComponent->GetAttributeSet(UMTD_BuilderSet::StaticClass());
    if (!IsValid(AttributeSet))
    {
        MTDS_WARN("Builder Set on PawnInstigator [%s] is invalid.", *PawnInstigator->GetName());
        return;
    }

    InstigatorBuilderSet = Cast<UMTD_BuilderSet>(AttributeSet);
}

bool AMTD_Tower::CheckTowerDataValidness() const
{
    if (!IsValid(TowerData))
    {
        MTDS_WARN("Tower datais invalid.");
        return false;
    }
    
    if (!IsValid(TowerData->AttributeTable))
    {
        MTDS_WARN("Attribute table on tower data [%s] is invalid.", *TowerData->GetName());
        return false;
    }
    
    if (!IsValid(TowerData->ProjectileData))
    {
        MTDS_WARN("Projectile data on tower data [%s] is invalid.", *TowerData->GetName());
        return false;
    }
    
    if (!TowerData->ProjectileData->ProjectileClass)
    {
        MTDS_WARN("Projectile class on projectile data [%s] is invalid.", *TowerData->ProjectileData->GetName());
        return false;
    }

    if (!TowerHealthAttributeScalingGeClass)
    {
        MTDS_WARN("Tower health attribute scaling gameplay effect class is invalid.");
        return false;
    }

    return true;
}

/**
 * Get direction from first argument towards the second one.
 * @param   Lhs: scene component the direction vector tail is located at.
 * @param   Rhs: scene component the direction vector tip is pointing at.
 * @return  Direction vector from Lhs to Rhs.
 */
static FVector GetDirectionTowards(const USceneComponent *Lhs, const USceneComponent *Rhs)
{
    check(IsValid(Lhs));
    check(IsValid(Rhs));
    
    const FVector P0 = Lhs->GetComponentLocation();
    const FVector P1 = Rhs->GetComponentLocation();
    const FVector DistanceVector = (P1 - P0);
    const FVector Direction = DistanceVector.GetSafeNormal();

    return Direction;
}

AMTD_Projectile *AMTD_Tower::SpawnProjectile(const FTransform &Transform)
{
    UWorld *World = GetWorld();
    check(IsValid(World));
    
    check(IsValid(TowerData));
    
    const UMTD_ProjectileData *ProjectileData = TowerData->ProjectileData;
    check(IsValid(ProjectileData));

    constexpr auto CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    auto Projectile = World->SpawnActorDeferred<AMTD_Projectile>(
        ProjectileData->ProjectileClass, Transform, this, this, CollisionHandlingMethod);
    
    if (!IsValid(Projectile))
    {
        MTDS_WARN("Failed to spawn a projectile.");
    }
    
    return Projectile;
}

void AMTD_Tower::SetupProjectile(AMTD_Projectile &Projectile, AActor *FireTarget)
{
    Projectile.InitializeAbilitySystem(GetAbilitySystemComponent());

    SetupProjectileCollision(Projectile);
    SetupProjectileMovement(Projectile, FireTarget);
    SetupProjectileGameplayEffectClasses(Projectile);

    Projectile.BalanceDamage = BalanceDamage;
}

void AMTD_Tower::SetupProjectileCollision(AMTD_Projectile &Projectile) const
{
    auto Collision = Projectile.GetCollisionComponent();

    // At the moment it's hardcoded since only player can have towers
    Collision->SetCollisionProfileName(AllyProjectileCollisionProfileName);
}

void AMTD_Tower::SetupProjectileMovement(AMTD_Projectile &Projectile, AActor *FireTarget) const
{
    UMTD_ProjectileMovementComponent *MovementComponent = Projectile.GetMovementComponent();
    check(IsValid(MovementComponent));

    const FVector Direction = GetDirectionTowards(Projectile.GetRootComponent(), FireTarget->GetRootComponent());
    const float Speed = GetScaledProjectileSpeed();
    USceneComponent *TargetRoot = FireTarget->GetRootComponent();

    MovementComponent->InitialSpeed = Speed;
    MovementComponent->MaxSpeed = Speed;
    MovementComponent->Direction = Direction;
    MovementComponent->HomingTargetComponent = TargetRoot;
}

void AMTD_Tower::SetupProjectileGameplayEffectClasses(AMTD_Projectile &Projectile) const
{
    check(IsValid(TowerData));
    
    const UMTD_ProjectileData *ProjectileData = TowerData->ProjectileData;
    check(IsValid(ProjectileData));
    
    Projectile.Damage = GetScaledDamage();
    Projectile.DamageAdditive = 0.f;
    Projectile.DamageMultiplier = 1.f;

    for (const TSubclassOf<UMTD_GameplayEffect> &Ge : ProjectileData->GameplayEffectsToGrantClasses)
    {
        Projectile.AddGameplayEffectClassToGrantOnHit(Ge);
    }
}

void AMTD_Tower::OnAbilitySystemInitialized()
{
    UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = GetMtdAbilitySystemComponent();
    check(IsValid(MtdAbilitySystemComponent));

    HealthComponent->InitializeWithAbilitySystem(MtdAbilitySystemComponent);
}

void AMTD_Tower::OnAbilitySystemUninitialized()
{
    HealthComponent->UninitializeFromAbilitySystem();
}

void AMTD_Tower::OnDeathStarted_Implementation(AActor *OwningActor)
{
    // Avoid running main logic when dead
    SetActorTickEnabled(false);

    // Avoid colliding with anything after death
    DisableCollision();
}

void AMTD_Tower::OnDeathFinished_Implementation(AActor *OwningActor)
{
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AMTD_Tower::DisableCollision()
{
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionComponent->SetCollisionResponseToChannels(ECR_Ignore);
}

void AMTD_Tower::FellOutOfWorld(const UDamageType &DamageType)
{
    HealthComponent->SelfDestruct(true);
}

void AMTD_Tower::DestroyDueToDeath()
{
    Uninit();
    DetachFromControllerPendingDestroy();
    SetLifeSpan(0.1f);
}

void AMTD_Tower::Uninit()
{
    // Uninit if we are still the avatar; we could be kicked from ASC if someone else owned it instead
    const UMTD_AbilitySystemComponent *AbilitySystemComponent = GetMtdAbilitySystemComponent();
    if (IsValid(AbilitySystemComponent))
    {
        if (AbilitySystemComponent->GetAvatarActor() == this)
        {
            PawnExtentionComponent->UninitializeAbilitySystem();
        }
    }

    SetActorHiddenInGame(true);
}
