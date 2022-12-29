#include "Projectile/MTD_Projectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/MTD_GameplayTags.h"
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
    const FGameplayEventData EventData = PrepareGameplayEventData(SweepResult);
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    
    OnProjectilePreHit(EventData);
    
    ApplyGameplayEffectsToTarget(OtherActor);
    
    AbilitySystemComponent->HandleGameplayEvent(GameplayTags.Gameplay_Event_RangeHit, &EventData);
    OnProjectilePostHit(EventData);
    
    Destroy();
}

void AMTD_Projectile::OnSelfDestroy_Implementation()
{
    Destroy();
}

void AMTD_Projectile::ApplyGameplayEffectsToTarget(AActor *Target)
{
    UAbilitySystemComponent *TargetAsc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
    if (!IsValid(TargetAsc))
    {
        return;
    }

    for (const FGameplayEffectSpecHandle &SpecHandle : GameplayEffectsToGrantOnHit)
    {
        if ((!SpecHandle.IsValid()) || (!SpecHandle.Data))
        {
            continue;
        }
        
        // TODO: Check if it's a radial projectile, apply GEs around if so

        FActiveGameplayEffectHandle ActiveGeHandle = TargetAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
        if (!ActiveGeHandle.WasSuccessfullyApplied())
        {
            MTDS_WARN("Failed to apply gameplay effect handle [%s] to [%s]",
                *ActiveGeHandle.ToString(), *Target->GetName());
        }
    }
}

void AMTD_Projectile::OnProjectilePreHit_Implementation(const FGameplayEventData &EventData)
{
    // Empty
}

void AMTD_Projectile::OnProjectilePostHit_Implementation(const FGameplayEventData &EventData)
{
    // Empty
}

FGameplayEventData AMTD_Projectile::PrepareGameplayEventData(FHitResult HitResult) const
{
    // For some reason sweep given by on overlap event doesn't contain crutial information about the transforms, hence
    // I fill some required information in here which is a bit inaccurate, but does the job
    
    const FVector ProjLocation = GetActorLocation();
    const FVector OtherLocation = HitResult.GetActor()->GetActorLocation();
    
    HitResult.Location = ProjLocation;
    HitResult.ImpactPoint = ProjLocation - OtherLocation;
    HitResult.ImpactNormal = FVector_NetQuantizeNormal(HitResult.ImpactPoint.GetUnsafeNormal());
    
    FGameplayEventData EventData;
    EventData.ContextHandle = AbilitySystemComponent->MakeEffectContext();
    EventData.Instigator = GetOwner(); // Should be PlayerState
    EventData.Target = HitResult.GetActor();
    EventData.TargetData.Data.Add(
        TSharedPtr<FGameplayAbilityTargetData>(new FGameplayAbilityTargetData_SingleTargetHit(HitResult)));

    return EventData;
}
