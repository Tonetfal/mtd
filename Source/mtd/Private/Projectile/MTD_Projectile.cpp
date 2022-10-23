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
	
	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(
		TEXT("Collision Capsule Component"));
	SetRootComponent(CollisionCapsule);
	CollisionCapsule->InitCapsuleSize(50.f, 50.f);
	CollisionCapsule->SetCollisionProfileName("NoCollision");
	CollisionCapsule->SetSimulatePhysics(false);
	CollisionCapsule->SetEnableGravity(false);

	ProjectileMovement =
		CreateDefaultSubobject<UMTD_ProjectileMovementComponent>
			(TEXT("MTD Projectile Movement Component"));
	ProjectileMovement->SetUpdatedComponent(CollisionCapsule);
}

void AMTD_Projectile::BeginPlay()
{
	Super::BeginPlay();

	check(ProjectileMovement);
	check(CollisionCapsule);

	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(
		this, &AMTD_Projectile::OnBeginOverlap);
}

void AMTD_Projectile::SetupProjectile(FMTD_ProjectileParameters Params)
{
	ProjectileParameters = Params;
		
	CollisionCapsule->SetCollisionProfileName(Params.CollisionProfileName.Name);
	
	GetWorldTimerManager().SetTimer(SelfDestroyTimerHandle,this,
		&AMTD_Projectile::OnSelfDestroy, SecondsToSelfDestroy);
	
	ProjectileMovement->SetMovementParameters(Params.MovementParameters);
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
	UAbilitySystemComponent *TargetAsc =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!IsValid(TargetAsc))
		return;

	for (const FGameplayEffectSpecHandle &SpecHandle : 
		ProjectileParameters.GameplayEffectsToGrantOnHit)
	{
		if (!SpecHandle.IsValid() || !SpecHandle.Data)
			continue;

		// TODO: Check if it's a radial projectile, apply GEs around if so

		FActiveGameplayEffectHandle ActiveGeHandle =
			TargetAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		if (!ActiveGeHandle.WasSuccessfullyApplied())
		{
			MTDS_WARN("Gameplay effect handle [%s] was not successfully "
				"applied to [%s]", *ActiveGeHandle.ToString(),
				*Target->GetName());
		}
	}
}
