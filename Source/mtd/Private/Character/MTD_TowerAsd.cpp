#include "Character/MTD_TowerAsd.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_HealthComponent.h"
#include "Character/MTD_HeroComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameModes/MTD_GameModeBase.h"
#include "Player/MTD_PlayerState.h"
#include "Player/MTD_TowerController.h"
#include "Projectile/MTD_Projectile.h"
#include "Projectile/MTD_ProjectileMovementComponent.h"

AMTD_TowerAsd::AMTD_TowerAsd()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

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

    bCanAffectNavigationGeneration = false;
}

void AMTD_TowerAsd::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

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

void AMTD_TowerAsd::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AMTD_TowerAsd::PostInitProperties()
{
    Super::PostInitProperties();
}

void AMTD_TowerAsd::BeginPlay()
{
    Super::BeginPlay();

    check(HealthComponent);

    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

    UWorld *World = GetWorld();
    AGameModeBase *Gm = World->GetAuthGameMode();
    if (IsValid(Gm))
    {
        auto MtdGm = CastChecked<AMTD_GameModeBase>(Gm);
        MtdGm->OnGameTerminatedDelegate.AddDynamic(this, &ThisClass::OnGameTerminated);
    }

    if (!IsValid(TowerData))
    {
        MTD_WARN("TowerData on owner [%s] is invalid", *GetName());
    }

    if (!IsValid(TowerData->AttributeTable))
    {
        MTD_WARN("AttributeTable on TowerData [%s] is invalid", *TowerData->GetName());
    }

    if (!TowerData->ProjectileClass)
    {
        MTD_WARN("ProjectileClass on TowerData [%s] is invalid", *TowerData->GetName());
    }

    if (!TowerData->DamageGameplayEffectClass)
    {
        MTD_WARN("DamageGameplayEffectClass on TowerData [%s] is invalid", *TowerData->GetName());
    }
}

void AMTD_TowerAsd::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AMTD_TowerAsd::Reset()
{
    Super::Reset();
}

void AMTD_TowerAsd::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    PawnExtentionComponent->HandleControllerChanged();
}

FMTD_AbilityAnimations AMTD_TowerAsd::GetAbilityAnimMontages(FGameplayTag AbilityTag) const
{
    return (IsValid(AnimationSet)) ? (AnimationSet->GetAbilityAnimMontages(AbilityTag)) : (FMTD_AbilityAnimations());
}

void AMTD_TowerAsd::FellOutOfWorld(const UDamageType &DamageType)
{
    HealthComponent->SelfDestruct(true);
}

float AMTD_TowerAsd::GetScaledDamage_Implementation()
{
    return BaseDamage;
}

float AMTD_TowerAsd::GetScaledFirerate_Implementation()
{
    return BaseFirerate;
}

float AMTD_TowerAsd::GetScaledVisionRange_Implementation()
{
    return BaseVisionRange;
}

float AMTD_TowerAsd::GetScaledVisionHalfDegrees_Implementation()
{
    return BaseVisionHalfDegrees;
}

float AMTD_TowerAsd::GetScaledProjectileSpeed_Implementation()
{
    return BaseProjectileSpeed;
}

float AMTD_TowerAsd::GetReloadTime_Implementation()
{
    // Fire 'BaseFirerate' times per second
    const float Firerate = GetScaledFirerate();
    return (Firerate == 0.f) ? (60.f) : (60.f / Firerate);
}

void AMTD_TowerAsd::OnFire(AActor *FireTarget)
{
    if (!IsValid(TowerData))
    {
        return;
    }

    AMTD_Projectile *Projectile = SpawnProjectile();
    if (!IsValid(Projectile))
    {
        return;
    }

    SetupProjectile(Projectile, FireTarget);
    StartReloading();
}

void AMTD_TowerAsd::StartReloading()
{
    check(!bIsReloading);

    bIsReloading = true;

    const float ReloadTime = GetReloadTime();
    GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ThisClass::OnReloadFinished, ReloadTime, false);
}

void AMTD_TowerAsd::OnReloadFinished()
{
    check(bIsReloading);

    bIsReloading = false;
}

FVector AMTD_TowerAsd::GetTargetDistanceVector(const USceneComponent *Projectile, const USceneComponent *Target) const
{
    const FVector P0 = Projectile->GetComponentLocation();
    const FVector P1 = Target->GetComponentLocation();
    const FVector DistanceVector = P1 - P0;

    return DistanceVector;
}

FVector AMTD_TowerAsd::GetTargetDirection(const USceneComponent *Projectile, const USceneComponent *Target) const
{
    const FVector DistanceVector = GetTargetDistanceVector(Projectile, Target);
    const FVector Direction = DistanceVector.GetSafeNormal();

    return Direction;
}

AMTD_Projectile *AMTD_TowerAsd::SpawnProjectile()
{
    if (!TowerData->ProjectileClass)
    {
        return nullptr;
    }

    UWorld *World = GetWorld();

    const FTransform Transform = ProjectileSpawnPosition->GetComponentTransform();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = SpawnParams.Instigator = this;

    AActor *Actor = World->SpawnActor(TowerData->ProjectileClass, &Transform, SpawnParams);
    auto Projectile = Cast<AMTD_Projectile>(Actor);

    return Projectile;
}

void AMTD_TowerAsd::SetupProjectile(AMTD_Projectile *Projectile, AActor *FireTarget)
{
    SetupProjectileCollision(Projectile);
    SetupProjectileMovement(Projectile, FireTarget);
    SetupProjectileEffectHandles(Projectile->GameplayEffectsToGrantOnHit);
}

void AMTD_TowerAsd::SetupProjectileCollision(AMTD_Projectile *Projectile) const
{
    auto Collision = Projectile->GetCollisionComponent();

    // At the moment it's hardcoded since only player can have towers
    Collision->SetCollisionProfileName(AllyProjectileCollisionProfileName);
}

void AMTD_TowerAsd::SetupProjectileMovement(AMTD_Projectile *Projectile, AActor *FireTarget)
{
    UMTD_ProjectileMovementComponent *MovementComponent = Projectile->GetMovementComponent();

    const FVector Direction = GetTargetDirection(Projectile->GetRootComponent(), FireTarget->GetRootComponent());
    const float Speed = GetScaledProjectileSpeed();

    MovementComponent->HomingTarget = FireTarget;
    MovementComponent->Direction = Direction;
    MovementComponent->MaxSpeed = Speed;
    MovementComponent->AddAcceleration(Speed);
}

void AMTD_TowerAsd::SetupProjectileEffectHandles(TArray<FGameplayEffectSpecHandle> &EffectHandles)
{
    // TODO: Maybe cache it instead of computing it every time. Cons are that GEs may vary depending on something, hence
    // some recalculation will be required, so a way of comunicating this request has to be created. It will increase
    // the performance since this method may be called hundred times per second.

    if (!TowerData->DamageGameplayEffectClass)
    {
        return;
    }

    UMTD_AbilitySystemComponent *Asc = GetMtdAbilitySystemComponent();
    FGameplayEffectContextHandle GeContext = Asc->MakeEffectContext();
    FGameplayEffectSpecHandle GeDmgSpec = Asc->MakeOutgoingSpec(TowerData->DamageGameplayEffectClass, 1.f, GeContext);
    EffectHandles.Add(GeDmgSpec);

    const FMTD_GameplayTags GameplayTags = FMTD_GameplayTags::Get();
    GeDmgSpec.Data->SetByCallerTagMagnitudes.Add(GameplayTags.SetByCaller_Damage_Additive, GetScaledDamage());
    GeDmgSpec.Data->SetByCallerTagMagnitudes.Add(GameplayTags.SetByCaller_Damage_Multiplier, 1.f);

    const TArray<TSubclassOf<UMTD_GameplayEffect>> &Ges = TowerData->GameplayEffectsToGrantClasses;
    for (const TSubclassOf<UMTD_GameplayEffect> &Ge : Ges)
    {
        EffectHandles.Add(Asc->MakeOutgoingSpec(Ge, 1.f, GeContext));
    }
}

void AMTD_TowerAsd::OnAbilitySystemInitialized()
{
    UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
    check(MtdAsc);

    HealthComponent->InitializeWithAbilitySystem(MtdAsc);
}

void AMTD_TowerAsd::OnAbilitySystemUninitialized()
{
    HealthComponent->UninitializeFromAbilitySystem();
}

void AMTD_TowerAsd::DestroyDueToDeath()
{
    DetachFromControllerPendingDestroy();
    SetLifeSpan(0.1f);

    Uninit();
}

void AMTD_TowerAsd::Uninit()
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

void AMTD_TowerAsd::OnDeathStarted_Implementation(AActor *OwningActor)
{
    SetActorTickEnabled(false);
    DisableCollision();
}

void AMTD_TowerAsd::OnDeathFinished_Implementation(AActor *OwningActor)
{
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AMTD_TowerAsd::DisableCollision()
{
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionComponent->SetCollisionResponseToChannels(ECR_Ignore);
}

AMTD_PlayerState *AMTD_TowerAsd::GetMtdPlayerState() const
{
    return CastChecked<AMTD_PlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UMTD_AbilitySystemComponent *AMTD_TowerAsd::GetMtdAbilitySystemComponent() const
{
    return PawnExtentionComponent->GetMtdAbilitySystemComponent();
}

UAbilitySystemComponent *AMTD_TowerAsd::GetAbilitySystemComponent() const
{
    return GetMtdAbilitySystemComponent();
}
