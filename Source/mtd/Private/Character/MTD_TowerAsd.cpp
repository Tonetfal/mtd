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
#include "Components/SphereComponent.h"
#include "Player/MTD_PlayerState.h"
#include "Player/MTD_TowerController.h"
#include "Projectile/MTD_Projectile.h"

AMTD_TowerAsd::AMTD_TowerAsd()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    AIControllerClass = AMTD_TowerController::StaticClass();

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));

    SetRootComponent(BoxComponent);
    BoxComponent->SetCollisionProfileName(AllyCollisionProfileName);

    ProjectileSpawnPosition = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Spawn Position"));

    ProjectileSpawnPosition->SetupAttachment(GetRootComponent());
    ProjectileSpawnPosition->SetCollisionProfileName("NoCollision");

    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));

    MeshComponent->SetupAttachment(GetRootComponent());
    MeshComponent->SetCollisionProfileName("NoCollision");

    PawnExtentionComponent = CreateDefaultSubobject<UMTD_PawnExtensionComponent>(TEXT("MTD Pawn Extension Component"));

    PawnExtentionComponent->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
    PawnExtentionComponent->OnAbilitySystemUninitialized_Register(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

    HeroComponent = CreateDefaultSubobject<UMTD_HeroComponent>(TEXT("MTD Hero Component"));
    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>(TEXT("MTD Health Component"));

    bCanAffectNavigationGeneration = false;
    BoxComponent->SetCanEverAffectNavigation(false);
    ProjectileSpawnPosition->SetCanEverAffectNavigation(false);
    MeshComponent->SetCanEverAffectNavigation(false);
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

    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

    if (!IsValid(TowerData))
        MTD_WARN("TowerData on owner [%s] is invalid", *GetName());

    if (!IsValid(TowerData->AttributeTable))
        MTD_WARN("AttributeTable on TowerData [%s] is invalid", *TowerData->GetName());
    
    if (!TowerData->ProjectileClass)
        MTD_WARN("ProjectileClass on TowerData [%s] is invalid", *TowerData->GetName());
    
    if (!TowerData->DamageGameplayEffectClass)
        MTD_WARN("DamageGameplayEffectClass on TowerData [%s] is invalid", *TowerData->GetName());
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
    return (BaseFirerate == 0.f) ? (60.f) : (60.f / BaseFirerate);
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

    const FMTD_ProjectileParameters Params = GetProjectileParameters(Projectile, FireTarget);
    Projectile->SetupProjectile(Params);

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

AMTD_Projectile *AMTD_TowerAsd::SpawnProjectile()
{
    if (!TowerData->ProjectileClass)
    {
        return nullptr;
    }

    UWorld *World = GetWorld();

    const FVector Location = ProjectileSpawnPosition->GetComponentLocation();
    const FRotator Rotation = GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = SpawnParams.Instigator = this;

    AActor *Actor = World->SpawnActor(TowerData->ProjectileClass, &Location, &Rotation, SpawnParams);
    auto Projectile = CastChecked<AMTD_Projectile>(Actor);

    return Projectile;
}

FMTD_ProjectileParameters AMTD_TowerAsd::GetProjectileParameters(AMTD_Projectile *Projectile, AActor *FireTarget)
{
    FMTD_ProjectileMovementParameters MoveParams = SetupProjectileMovement(Projectile, FireTarget);
    TArray<FGameplayEffectSpecHandle> GeSpecHandles = GetProjectileEffects();

    FMTD_ProjectileParameters Params;
    Params.MovementParameters = MoveParams;
    Params.GameplayEffectsToGrantOnHit = GeSpecHandles;

    // At the moment it's hardcoded since only player can have towers
    Params.CollisionProfileName = AllyProjectileCollisionProfileName;

    return Params;
}

FMTD_ProjectileMovementParameters AMTD_TowerAsd::SetupProjectileMovement(AMTD_Projectile *Projectile,
    AActor *FireTarget)
{
    FMTD_ProjectileMovementParameters Params = TowerData->ProjectileParameters.MovementParameters;

    const FVector P0 = Projectile->GetActorLocation();
    const FVector P1 = FireTarget->GetActorLocation();
    const FVector Dist = P1 - P0;
    const FVector Dir = Dist.GetSafeNormal();

    Params.HomingTarget = FireTarget;
    Params.Direction = Dir;
    Params.InitialSpeed = Params.MaxSpeed = GetScaledProjectileSpeed();

    return Params;
}

TArray<FGameplayEffectSpecHandle> AMTD_TowerAsd::GetProjectileEffects()
{
    // Maybe cache it instead of computing it every time. Cons are that GEs may vary depending on something, hence
    // some recalculation will be required, so a way of comunicating this request has to be created. It will increase
    // the performance since this method may be called even hundred times per second.

    TArray<FGameplayEffectSpecHandle> Result;

    if (!TowerData->DamageGameplayEffectClass)
    {
        return Result;
    }

    UMTD_AbilitySystemComponent *Asc = GetMtdAbilitySystemComponent();
    FGameplayEffectContextHandle GeContext = Asc->MakeEffectContext();
    FGameplayEffectSpecHandle GeDmgSpec = Asc->MakeOutgoingSpec(TowerData->DamageGameplayEffectClass, 1.f, GeContext);
    Result.Add(GeDmgSpec);

    const FMTD_GameplayTags GameplayTags = FMTD_GameplayTags::Get();
    GeDmgSpec.Data->SetByCallerTagMagnitudes.Add(GameplayTags.SetByCaller_Damage_Additive, GetScaledDamage());
    GeDmgSpec.Data->SetByCallerTagMagnitudes.Add(GameplayTags.SetByCaller_Damage_Multiplier, 1.f);

    const TArray<TSubclassOf<UMTD_GameplayEffect>> &Ges = TowerData->GameplayEffectsToGrantClasses;
    for (const TSubclassOf<UMTD_GameplayEffect> &Ge : Ges)
    {
        Result.Add(Asc->MakeOutgoingSpec(Ge, 1.f, GeContext));
    }

    return Result;
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
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BoxComponent->SetCollisionResponseToChannels(ECR_Ignore);
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
