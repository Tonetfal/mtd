#include "Character/MTD_Tower.h"

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
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

AMTD_Tower::AMTD_Tower()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    AIControllerClass = AMTD_TowerController::StaticClass();

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Component"));
    SetRootComponent(CollisionComponent);

    CollisionComponent->SetCollisionProfileName(TowerCollisionProfileName);
    CollisionComponent->SetSimulatePhysics(false);
    CollisionComponent->SetEnableGravity(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    NavVolumeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Navigation Volume Component"));
    NavVolumeComponent->SetupAttachment(GetRootComponent());

    NavVolumeComponent->SetCollisionProfileName("NoCollision");
    NavVolumeComponent->SetSimulatePhysics(false);
    NavVolumeComponent->SetEnableGravity(false);
    NavVolumeComponent->SetCanEverAffectNavigation(true);
    NavVolumeComponent->bDynamicObstacle = true;

    ProjectileSpawnPosition = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Spawn Position"));
    ProjectileSpawnPosition->SetupAttachment(GetRootComponent());

    ProjectileSpawnPosition->SetCollisionProfileName("NoCollision");
    ProjectileSpawnPosition->SetSimulatePhysics(false);
    ProjectileSpawnPosition->SetEnableGravity(false);
    ProjectileSpawnPosition->SetCanEverAffectNavigation(false);

    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
    MeshComponent->SetupAttachment(GetRootComponent());

    MeshComponent->SetCollisionProfileName("NoCollision");
    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetEnableGravity(false);
    MeshComponent->SetCanEverAffectNavigation(false);

    PawnExtentionComponent = CreateDefaultSubobject<UMTD_PawnExtensionComponent>(TEXT("MTD Pawn Extension Component"));

    PawnExtentionComponent->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
    PawnExtentionComponent->OnAbilitySystemUninitialized_Register(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

    HeroComponent = CreateDefaultSubobject<UMTD_HeroComponent>(TEXT("MTD Hero Component"));
    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>(TEXT("MTD Health Component"));
    TowerExtensionComponent = CreateDefaultSubobject<UMTD_TowerExtensionComponent>(
        TEXT("MTD Tower Extension Component"));

    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

    bCanAffectNavigationGeneration = false;
}

void AMTD_Tower::BeginPlay()
{
    Super::BeginPlay();

    check(HealthComponent);

    UWorld *World = GetWorld();
    AGameModeBase *Gm = World->GetAuthGameMode();
    if (IsValid(Gm))
    {
        auto MtdGm = CastChecked<AMTD_GameModeBase>(Gm);
        MtdGm->OnGameTerminatedDelegate.AddDynamic(this, &ThisClass::OnGameTerminated);
    }

    const auto Data = TowerExtensionComponent->GetTowerData<UMTD_TowerData>();

    if (!IsValid(Data))
    {
        MTD_WARN("TowerData on owner [%s] is invalid.", *GetName());
    }
    else
    {
        if (!IsValid(Data->AttributeTable))
        {
            MTD_WARN("AttributeTable on TowerData [%s] is invalid.", *Data->GetName());
            return;
        }
        
        if (IsValid(Data->ProjectileData))
        {
            if (!Data->ProjectileData->ProjectileClass)
            {
                MTD_WARN("ProjectileClass on ProjectileData [%s] is invalid.", *Data->ProjectileData->GetName());
                return;
            }

            if (!Data->ProjectileData->DamageGameplayEffectClass)
            {
                MTD_WARN("DamageGameplayEffectClass on ProjectileData [%s] is invalid.",
                    *Data->ProjectileData->GetName());
                return;
            }
        }
        else
        {
            MTD_WARN("ProjectileData on TowerData [%s] is invalid.", *Data->GetName());
        }
    }

    InitializeAttributes();

    // Disabled by default
    SetActorTickEnabled(true);
}

void AMTD_Tower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

#ifdef WITH_EDITOR
    const auto TowerData = TowerExtensionComponent->GetTowerData<UMTD_TowerData>();
    ensure(TowerData);
    ensure(TowerData->ProjectileData);
#endif

    if ((bIsReloading) || (!IsValid(Controller)))
    {
        return;
    }

    auto TowerController = CastChecked<AMTD_TowerController>(Controller);
    AActor *FireTarget = TowerController->GetFireTarget();

    if (!IsValid(FireTarget))
    {
        return;
    }

    OnFire(FireTarget);
}

void AMTD_Tower::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AMTD_Tower::PostInitProperties()
{
    Super::PostInitProperties();
}

void AMTD_Tower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AMTD_Tower::Reset()
{
    Super::Reset();
}

void AMTD_Tower::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    PawnExtentionComponent->HandleControllerChanged();
}

void AMTD_Tower::FellOutOfWorld(const UDamageType &DamageType)
{
    HealthComponent->SelfDestruct(true);
}

float AMTD_Tower::GetScaledDamage_Implementation() const
{
    return BaseDamage;
}

float AMTD_Tower::GetScaledFirerate_Implementation() const
{
    return BaseFirerate;
}

float AMTD_Tower::GetScaledVisionRange_Implementation() const
{
    return BaseVisionRange;
}

float AMTD_Tower::GetScaledVisionHalfDegrees_Implementation() const
{
    return BaseVisionHalfDegrees;
}

float AMTD_Tower::GetScaledProjectileSpeed_Implementation() const
{
    return BaseProjectileSpeed;
}

float AMTD_Tower::GetReloadTime_Implementation() const
{
    // Fire 'BaseFirerate' times per second
    const float Firerate = GetScaledFirerate();
    return (Firerate == 0.f) ? (60.f) : (60.f / Firerate);
}

void AMTD_Tower::InitializeAttributes()
{
    // TowerData with its attribute table must be validated by BeginPlay
    const auto TowerData = TowerExtensionComponent->GetTowerData<UMTD_TowerData>();

    // Dispatch the Attribute Table regardless the ASC presence due SBS. The SBS may spawn a tower without a controller,
    // hence the ASC will not be initialized. However, the reason SBS may spawn it is because it needs to retrieve
    // vision related data along the owner player stats. The spawned tower will have PlayerCharacter as Owner and its
    // PlayerState as Instigator.

    float Value;

    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, DamageAttributeName, Level, Value);
    BaseDamage = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, RangeAttributeName, Level, Value);
    BaseVisionRange = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, VisionDegreesAttributeName, Level, Value);
    BaseVisionHalfDegrees = Value / 2.f;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, FirerateAttributeName, Level, Value);
    BaseFirerate = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, ProjectileSpeedAttributeName, Level, Value);
    BaseProjectileSpeed = Value;
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, BalanceDamageAttributeName, Level, Value);
    BalanceDamage = Value;

    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    if (!IsValid(Asc))
    {
        MTDS_WARN("Ability System Component on Tower [%s] is invalid.", *GetName());
        return;
    }
    
    EVALUTE_ATTRIBUTE(TowerData->AttributeTable, HealthAttributeName, Level, Value);
    Asc->ApplyModToAttribute(UMTD_HealthSet::GetMaxHealthAttribute(), EGameplayModOp::Type::Override, Value);
    Asc->ApplyModToAttribute(UMTD_HealthSet::GetHealthAttribute(), EGameplayModOp::Type::Override, Value);
    Asc->ApplyModToAttribute(UMTD_BalanceSet::GetBaseDamageAttribute(), EGameplayModOp::Type::Override, BalanceDamage);

    // Tower ignore any balance damage
    Asc->ApplyModToAttribute(UMTD_BalanceSet::GetResistAttribute(), EGameplayModOp::Type::Override, 100.f);
    

    OnAttributesChanged.Broadcast();
    MTDS_VERBOSE("Tower [%s]'s attributes have been initialized.", *GetName());
}

void AMTD_Tower::OnGameTerminated_Implementation(EMTD_GameResult GameResult)
{
    IMTD_GameResultInterface::OnGameTerminated_Implementation(GameResult);
    DetachFromControllerPendingDestroy();
}

void AMTD_Tower::OnFire(AActor *FireTarget)
{
    if (!IsValid(TowerExtensionComponent->GetTowerData<UMTD_TowerData>()))
    {
        return;
    }

    AMTD_Projectile *Projectile = SpawnProjectile();
    if (!IsValid(Projectile))
    {
        return;
    }

    SetupProjectile(*Projectile, FireTarget);
    StartReloading();
}

void AMTD_Tower::StartReloading()
{
    check(!bIsReloading);

    bIsReloading = true;

    const float ReloadTime = GetReloadTime();
    GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ThisClass::OnReloadFinished, ReloadTime, false);
}

void AMTD_Tower::OnReloadFinished()
{
    check(bIsReloading);

    bIsReloading = false;
}

FVector AMTD_Tower::GetTargetDistanceVector(const USceneComponent *Projectile, const USceneComponent *Target) const
{
    const FVector P0 = Projectile->GetComponentLocation();
    const FVector P1 = Target->GetComponentLocation();
    const FVector DistanceVector = P1 - P0;

    return DistanceVector;
}

FVector AMTD_Tower::GetTargetDirection(const USceneComponent *Projectile, const USceneComponent *Target) const
{
    const FVector DistanceVector = GetTargetDistanceVector(Projectile, Target);
    const FVector Direction = DistanceVector.GetSafeNormal();

    return Direction;
}

AMTD_Projectile *AMTD_Tower::SpawnProjectile()
{
    UWorld *World = GetWorld();
    const auto TowerData = TowerExtensionComponent->GetTowerData<UMTD_TowerData>();
    const UMTD_ProjectileData *ProjectileData = TowerData->ProjectileData;

    const FTransform Transform = ProjectileSpawnPosition->GetComponentTransform();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = SpawnParams.Instigator = this;

    AActor *Actor = World->SpawnActor(ProjectileData->ProjectileClass, &Transform, SpawnParams);
    auto Projectile = Cast<AMTD_Projectile>(Actor);

    return Projectile;
}

void AMTD_Tower::SetupProjectile(AMTD_Projectile &Projectile, AActor *FireTarget)
{
    Projectile.InitializeAbilitySystem(GetAbilitySystemComponent());
    
    SetupProjectileCollision(Projectile);
    SetupProjectileMovement(Projectile, FireTarget);
    SetupProjectileHitCallback();
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

    const FVector Direction = GetTargetDirection(Projectile.GetRootComponent(), FireTarget->GetRootComponent());
    const float Speed = GetScaledProjectileSpeed();

    MovementComponent->HomingTarget = FireTarget;
    MovementComponent->Direction = Direction;
    MovementComponent->MaxSpeed = Speed;
    MovementComponent->AddAcceleration(Speed);
}

void AMTD_Tower::SetupProjectileGameplayEffectClasses(AMTD_Projectile &Projectile) const
{
    const auto TowerData = TowerExtensionComponent->GetTowerData<UMTD_TowerData>();
    const UMTD_ProjectileData *ProjectileData = TowerData->ProjectileData;
    
    Projectile.Damage = GetScaledDamage();
    Projectile.DamageMultiplier = 1.f;

    Projectile.SetGameplayEffectDamageClass(ProjectileData->DamageGameplayEffectClass);
    for (const TSubclassOf<UMTD_GameplayEffect> &Ge : ProjectileData->GameplayEffectsToGrantClasses)
    {
        Projectile.AddGameplayEffectClassToGrantOnHit(Ge);
    }
}

void AMTD_Tower::SetupProjectileHitCallback()
{
    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    
    FGameplayEventMulticastDelegate Delegate;
    Delegate.AddUObject(this, &ThisClass::OnProjectileHit);
    
    Asc->GenericGameplayEventCallbacks.Add(GameplayTags.Gameplay_Event_RangeHit, Delegate);
}

void AMTD_Tower::OnProjectileHit(const FGameplayEventData *EventData)
{
    check(EventData);
    K2_OnProjectileHit(*EventData);
}

void AMTD_Tower::OnAbilitySystemInitialized()
{
    UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
    check(MtdAsc);

    HealthComponent->InitializeWithAbilitySystem(MtdAsc);
}

void AMTD_Tower::OnAbilitySystemUninitialized()
{
    HealthComponent->UninitializeFromAbilitySystem();
}

void AMTD_Tower::DestroyDueToDeath()
{
    DetachFromControllerPendingDestroy();
    SetLifeSpan(0.1f);

    Uninit();
}

void AMTD_Tower::Uninit()
{
    // Uninit if we are still the avatar; we could be kicked from ASC if someone else owned it instead
    const UMTD_AbilitySystemComponent *Asc = GetMtdAbilitySystemComponent();
    if (IsValid(Asc))
    {
        if (Asc->GetAvatarActor() == this)
        {
            PawnExtentionComponent->UninitializeAbilitySystem();
        }
    }

    SetActorHiddenInGame(true);
}

void AMTD_Tower::OnDeathStarted_Implementation(AActor *OwningActor)
{
    SetActorTickEnabled(false);
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
