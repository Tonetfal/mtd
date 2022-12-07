#include "Projectile/MTD_Projectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
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

    MovementComponent = CreateDefaultSubobject<UMTD_ProjectileMovementComponent>(TEXT("Movement Component"));
    MovementComponent->SetUpdatedComponent(GetRootComponent());
}

void AMTD_Projectile::BeginPlay()
{
    Super::BeginPlay();

    check(MovementComponent);
    check(CollisionComponent);

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMTD_Projectile::OnBeginOverlap);

    GetWorldTimerManager().SetTimer(
        SelfDestroyTimerHandle, this, &AMTD_Projectile::OnSelfDestroy, SecondsToSelfDestroy);
}

void AMTD_Projectile::OnBeginOverlap(
    UPrimitiveComponent *HitComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    ApplyGameplayEffectToTarget(OtherActor);
    Destroy();
}

void AMTD_Projectile::OnSelfDestroy_Implementation()
{
    if (SelfDestroyTimerHandle.IsValid())
    {
        GetWorldTimerManager().ClearTimer(SelfDestroyTimerHandle);
    }

    Destroy();
}

void AMTD_Projectile::ApplyGameplayEffectToTarget(AActor *Target)
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
