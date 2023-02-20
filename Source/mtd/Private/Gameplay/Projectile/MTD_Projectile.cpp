#include "Gameplay/Projectile/MTD_Projectile.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "Components/CapsuleComponent.h"
#include "Gameplay/Projectile/MTD_ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AMTD_Projectile::AMTD_Projectile()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>("Collision Component");
    SetRootComponent(CollisionComponent);
    CollisionComponent->InitCapsuleSize(50.f, 50.f);
    CollisionComponent->SetCollisionProfileName("NoCollision");
    CollisionComponent->SetSimulatePhysics(false);
    CollisionComponent->SetEnableGravity(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    MovementComponent = CreateDefaultSubobject<UMTD_ProjectileMovementComponent>("Movement Component");
    MovementComponent->SetUpdatedComponent(GetRootComponent());
}

void AMTD_Projectile::InitializeAbilitySystem(UAbilitySystemComponent *InAbilitySystemComponent)
{
    check(IsValid(InAbilitySystemComponent));
    AbilitySystemComponent = InAbilitySystemComponent;
}

void AMTD_Projectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Listen for hit events
    CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnCollisionHit);
}

void AMTD_Projectile::BeginPlay()
{
    Super::BeginPlay();

    check(IsValid(MovementComponent));
    check(IsValid(CollisionComponent));

    // Destroy ownself after TimeToSelfDestroy seconds
    FTimerHandle SelfDestroyTimerHandle;
    GetWorldTimerManager().SetTimer(SelfDestroyTimerHandle, this, &ThisClass::OnSelfDestroy, TimeToSelfDestroy);
}

void AMTD_Projectile::OnCollisionHit(
    UPrimitiveComponent *HitComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    FVector NormalImpulse,
    const FHitResult &Hit)
{
    const FGameplayEventData EventData = PrepareGameplayEventData(Hit);

    // Dispatch gameplay event data
    OnProjectilePreHit(EventData);
    OnProjectileHit(EventData);
    OnProjectilePostHit(EventData);
}

void AMTD_Projectile::OnProjectilePreHit_Implementation(const FGameplayEventData &EventData)
{
    // Empty
}

void AMTD_Projectile::OnProjectileHit_Implementation(const FGameplayEventData &EventData)
{
    TArray<AActor *> Targets;
    if (bIsRadial)
    {
        SweepActorsAround(Targets);
    }
    else
    {
        // Target only actors that implement ASC interface
        AActor *HitActor = const_cast<AActor *>(EventData.Target.Get());
        if (Cast<IAbilitySystemInterface>(HitActor))
        {
            Targets.Add(HitActor);
        }
    }

    // Avoid preparing data if there is nothing to hit
    if (!Targets.IsEmpty())
    {
        PrepareGameplayEffectSpecHandles(EventData);
    }

    // Apply everything on targets
    for (AActor *Actor : Targets)
    {
        ApplyGameplayEffectsToTarget(Actor);
    }

    // Send a gameplay event
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    AbilitySystemComponent->HandleGameplayEvent(GameplayTags.Gameplay_Event_RangeHit, &EventData);
}

void AMTD_Projectile::OnProjectilePostHit_Implementation(const FGameplayEventData &EventData)
{
    Destroy();
}

void AMTD_Projectile::OnSelfDestroy_Implementation()
{
    // Time has expired
    Destroy();
}

void AMTD_Projectile::ApplyGameplayEffectsToTarget(AActor *Target)
{
    UAbilitySystemComponent *TargetAbilitySystemComponent =
        UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
    check(IsValid(TargetAbilitySystemComponent));

    // Iterate through all gameplay effect spec handles, and grant them on ASC
    for (const FGameplayEffectSpecHandle &SpecHandle : GameplayEffectsToGrantOnHit)
    {
        if (((!SpecHandle.IsValid()) || (!SpecHandle.Data)))
        {
            continue;
        }

        // Apply gameplay effect spec handle
        FActiveGameplayEffectHandle ActiveGeHandle =
            TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
        if (!ActiveGeHandle.WasSuccessfullyApplied())
        {
            MTDS_WARN("Failed to apply gameplay effect handle [%s] to [%s]",
                *ActiveGeHandle.ToString(), *Target->GetName());
        }
    }
}

void AMTD_Projectile::SweepActorsAround(TArray<AActor *> &OutActors) const
{
    ensure(bIsRadial);

    // Prepare sweep data
    const UWorld *World = GetWorld();
    const FVector SweepLocation = GetActorLocation();
    const FName CollisionProfileName = CollisionComponent->GetCollisionProfileName();
    const TArray<AActor*> ActorsToIgnore;
    const EDrawDebugTrace::Type DrawDebugTrace =
        ((bDebugSweep) ? (EDrawDebugTrace::ForDuration) : (EDrawDebugTrace::None));

    // Sweep around
    TArray<FHitResult> OutHits;
    UKismetSystemLibrary::SphereTraceMultiByProfile(
        World, SweepLocation, SweepLocation, RadialRange, CollisionProfileName, false, ActorsToIgnore,
        DrawDebugTrace, OutHits, false, FLinearColor::Red, FLinearColor::Green, SweepDrawTime);

    // Check which actors have an ability system component
    for (const FHitResult &HitResult : OutHits)
    {
        AActor *HitActor = HitResult.GetActor();
        if (IsValid(HitActor))
        {
            // Target only actors that implement ability system component interface
            if (Cast<IAbilitySystemInterface>(HitActor))
            {
                OutActors.Add(HitActor);
            }
        }
    }
}

void AMTD_Projectile::PrepareGameplayEffectSpecHandles(const FGameplayEventData &EventData)
{
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability system component is invalid.");
        return;
    }

    const FGameplayAbilityTargetData *TargetData = EventData.TargetData.Data[0].Get();
    if (!TargetData)
    {
        MTDS_WARN("Target data for target [0] is NULL.");
        return;
    }
    
    const FHitResult *HitResult = TargetData->GetHitResult();
    if (!HitResult)
    {
        MTDS_WARN("Hit Result is NULL.");
        return;
    }

    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddHitResult(*HitResult, true);

    const FVector KnockBackDirection = ComputeKnockBackDirection();
    const FGameplayEffectSpecHandle &DamageSpec = GetDamageSpecHandle(EffectContext);
    const FGameplayEffectSpecHandle &BalanceDamageSpec = GetBalanceDamageSpecHandle(KnockBackDirection, EffectContext);
    
    // Apply balance set beforehand because it may play some animations that are less important than death animation
    GameplayEffectsToGrantOnHit.Add(BalanceDamageSpec);
    
    // Apply damage spec the last because it may play death animation, which is the most important one
    GameplayEffectsToGrantOnHit.Add(DamageSpec);
}

FGameplayEventData AMTD_Projectile::PrepareGameplayEventData(const FHitResult &HitResult) const
{
    FGameplayEventData EventData;
    EventData.ContextHandle = AbilitySystemComponent->MakeEffectContext();
    EventData.Instigator = GetOwner(); // Should be PlayerState
    EventData.Target = HitResult.GetActor();
    EventData.TargetData.Data.Add(
        TSharedPtr<FGameplayAbilityTargetData>(new FGameplayAbilityTargetData_SingleTargetHit(HitResult)));

    return EventData;
}

FGameplayEffectSpecHandle AMTD_Projectile::GetBalanceDamageSpecHandle(const FVector KnockBackDirection,
    const FGameplayEffectContextHandle &EffectContext) const
{
    FGameplayEffectSpecHandle SpecHandle;
    if (!GameplayEffectBalanceDamageInstantClass)
    {
        MTDS_WARN("Balance damage instant gameplay effect class is invalid.");
        return SpecHandle;
    }

    // Create spec handle
    check(IsValid(AbilitySystemComponent));
    SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectBalanceDamageInstantClass, 1.f, EffectContext);
    
    // Fill up knock back and balance information
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_KnockbackDirection_X, KnockBackDirection.X);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_KnockbackDirection_Y, KnockBackDirection.Y);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_KnockbackDirection_Z, KnockBackDirection.Z);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Balance_Damage, BalanceDamage);

    return SpecHandle;
}

FGameplayEffectSpecHandle AMTD_Projectile::GetDamageSpecHandle(
    const FGameplayEffectContextHandle &EffectContext) const
{
    FGameplayEffectSpecHandle SpecHandle;
    if (!GameplayEffectDamageInstantClass)
    {
        MTDS_WARN("Damage instant gameplay effect class is invalid.");
        return SpecHandle;
    }

    // Create spec handle
    check(IsValid(AbilitySystemComponent));
    SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectDamageInstantClass, 1.f, EffectContext);

    // Fill up damage information
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Damage_Base, Damage);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Damage_Additive, DamageAdditive);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Damage_Multiplier, DamageMultiplier);

    return SpecHandle;
}

FVector AMTD_Projectile::ComputeKnockBackDirection() const
{
    const FVector Forward = GetActorForwardVector();
    return Forward;
}
