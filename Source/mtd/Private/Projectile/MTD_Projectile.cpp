#include "Projectile/MTD_Projectile.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile/MTD_ProjectileMovementComponent.h"

AMTD_Projectile::AMTD_Projectile()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Component"));
    SetRootComponent(CollisionComponent);

    CollisionComponent->InitCapsuleSize(50.f, 50.f);
    CollisionComponent->SetCollisionProfileName("NoCollision");
    CollisionComponent->SetSimulatePhysics(false);
    CollisionComponent->SetEnableGravity(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);

    MovementComponent = CreateDefaultSubobject<UMTD_ProjectileMovementComponent>(TEXT("Movement Component"));
    MovementComponent->SetUpdatedComponent(GetRootComponent());
}

void AMTD_Projectile::InitializeAbilitySystem(UAbilitySystemComponent *InAbilitySystemComponent)
{
    check(InAbilitySystemComponent);
    AbilitySystemComponent = InAbilitySystemComponent;
}

void AMTD_Projectile::AddGameplayEffectClassToGrantOnHit(const TSubclassOf<UMTD_GameplayEffect> &GeClass)
{
    GameplayEffectClassesToGrantOnHit.Add(GeClass);
}

void AMTD_Projectile::SetGameplayEffectDamageClass(const TSubclassOf<UMTD_GameplayEffect> &GeClass)
{
    GameplayEffectDamageClass = GeClass;
}

void AMTD_Projectile::BeginPlay()
{
    Super::BeginPlay();

    check(MovementComponent);
    check(CollisionComponent);

    FTimerHandle SelfDestroyTimerHandle;
    GetWorldTimerManager().SetTimer(SelfDestroyTimerHandle, this, &ThisClass::OnSelfDestroy, SecondsToSelfDestroy);
}

void AMTD_Projectile::OnBeginOverlap(
    UPrimitiveComponent *HitComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    const FGameplayEventData EventData = PrepareGameplayEventData(OtherActor);
    
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

    for (AActor *Actor : Targets)
    {
        ApplyGameplayEffectsToTarget(Actor);
    }
    
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    AbilitySystemComponent->HandleGameplayEvent(GameplayTags.Gameplay_Event_RangeHit, &EventData);
}

void AMTD_Projectile::OnProjectilePostHit_Implementation(const FGameplayEventData &EventData)
{
    Destroy();
}

void AMTD_Projectile::OnSelfDestroy_Implementation()
{
    Destroy();
}

void AMTD_Projectile::ApplyGameplayEffectsToTarget(AActor *Target)
{
    UAbilitySystemComponent *TargetAsc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
    check(IsValid(TargetAsc));

    for (const FGameplayEffectSpecHandle &SpecHandle : GameplayEffectsToGrantOnHit)
    {
        if (((!SpecHandle.IsValid()) || (!SpecHandle.Data)))
        {
            continue;
        }
        
        FActiveGameplayEffectHandle ActiveGeHandle = TargetAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
        if (!ActiveGeHandle.WasSuccessfullyApplied())
        {
            MTDS_WARN("Failed to apply gameplay effect handle [%s] to [%s]",
                *ActiveGeHandle.ToString(), *Target->GetName());
        }
    }
}

void AMTD_Projectile::SweepActorsAround(TArray<AActor *> &Actors) const
{
    ensure(bIsRadial);
    
    const UWorld *World = GetWorld();
    const FVector SweepLocation = GetActorLocation();
    const FName CollisionProfileName = CollisionComponent->GetCollisionProfileName();
    const TArray<AActor*> ActorsToIgnore;
    const EDrawDebugTrace::Type DrawDebugTrace =
        ((bDebugSweep) ? (EDrawDebugTrace::ForDuration) : (EDrawDebugTrace::None));

    TArray<FHitResult> OutHits;
    UKismetSystemLibrary::SphereTraceMultiByProfile(
        World, SweepLocation, SweepLocation, RadialRange, CollisionProfileName, false, ActorsToIgnore,
        DrawDebugTrace, OutHits, false, FLinearColor::Red, FLinearColor::Green, SweepDrawTime);

    for (const FHitResult &HitResult : OutHits)
    {
        AActor *HitActor = HitResult.GetActor();
        if (IsValid(HitActor))
        {
            // Target only actors that implement ASC interface
            if (Cast<IAbilitySystemInterface>(HitActor))
            {
                Actors.Add(HitActor);
            }
        }
    }
}

void AMTD_Projectile::PrepareGameplayEffectSpecHandles(const FGameplayEventData &EventData)
{
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability System Component is invalid.");
        return;
    }

    const FGameplayAbilityTargetData *TargetData = EventData.TargetData.Data[0].Get();
    if (!TargetData)
    {
        MTDS_WARN("Target Data for Target (0) is NULL.");
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

    const FVector KnockbackDirection = ComputeKnockbackDirection();
    const FGameplayEffectSpecHandle &DamageSpec = GetDamageSpecHandle(EffectContext);
    const FGameplayEffectSpecHandle &BalanceDamageSpec = GetBalanceDamageSpecHandle(KnockbackDirection, EffectContext);
    
    // Apply balance set beforehand because it may play some animations that are less important than death animation
    GameplayEffectsToGrantOnHit.Add(BalanceDamageSpec);
    
    // Apply damage spec the last because it may play death animation, which is the most important one
    GameplayEffectsToGrantOnHit.Add(DamageSpec);
}

FGameplayEventData AMTD_Projectile::PrepareGameplayEventData(const AActor *HitActor) const
{
    // For some reason sweep given by on overlap event doesn't contain crutial information about the transforms, hence
    // manually fill some required information in here which is a bit inaccurate, but does the job
    
    const FVector ProjLocation = GetActorLocation();
    const FVector OtherLocation = HitActor->GetActorLocation();

    FHitResult HitResult;
    HitResult.Location = ProjLocation;
    HitResult.ImpactPoint = (ProjLocation - OtherLocation);
    HitResult.ImpactNormal = FVector_NetQuantizeNormal(HitResult.ImpactPoint.GetUnsafeNormal());
    
    FGameplayEventData EventData;
    EventData.ContextHandle = AbilitySystemComponent->MakeEffectContext();
    EventData.Instigator = GetOwner(); // Should be PlayerState
    EventData.Target = HitActor;
    EventData.TargetData.Data.Add(
        TSharedPtr<FGameplayAbilityTargetData>(new FGameplayAbilityTargetData_SingleTargetHit(HitResult)));

    return EventData;
}

FGameplayEffectSpecHandle AMTD_Projectile::GetBalanceDamageSpecHandle(const FVector KnockbackDirection,
    const FGameplayEffectContextHandle &EffectContext) const
{
    FGameplayEffectSpecHandle SpecHandle;
    if (!GameplayEffectBalanceDamageInstantClass)
    {
        MTDS_WARN("Balance Damage Instant Gameplay Effect Class is invalid.");
        return SpecHandle;
    }

    check(IsValid(AbilitySystemComponent));
    SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectBalanceDamageInstantClass, 1.f, EffectContext);
    
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_KnockbackDirection_X, KnockbackDirection.X);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_KnockbackDirection_Y, KnockbackDirection.Y);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_KnockbackDirection_Z, KnockbackDirection.Z);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Balance_Damage, BalanceDamage);

    return SpecHandle;
}

FGameplayEffectSpecHandle AMTD_Projectile::GetDamageSpecHandle(
    const FGameplayEffectContextHandle &EffectContext) const
{
    FGameplayEffectSpecHandle SpecHandle;
    if (!GameplayEffectDamageInstantClass)
    {
        MTDS_WARN("Damage Instant Gameplay Effect Class is invalid.");
        return SpecHandle;
    }

    check(IsValid(AbilitySystemComponent));
    SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectDamageInstantClass, 1.f, EffectContext);

    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Damage_Base, Damage);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Damage_Additive, DamageAdditive);
    SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.SetByCaller_Damage_Multiplier, DamageMultiplier);

    return SpecHandle;
}

FVector AMTD_Projectile::ComputeKnockbackDirection() const
{
    const FVector Forward = GetActorForwardVector();
    return Forward;
}
