#include "Projectile/MTD_Projectile.h"

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
			(TEXT("MTD Movement Component"));
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

void AMTD_Projectile::SetupParameters(
	FMTD_ProjectileParameters ProjectileParameters)
{
	Parameters = ProjectileParameters;
	
	ProjectileMovement->SetInitialSpeed(Parameters.InitialSpeed);
	ProjectileMovement->SetMaxSpeed(Parameters.MaxSpeed);
	ProjectileMovement->SetAcceleration(Parameters.Acceleration);
	ProjectileMovement->SetRotationFollowsVelocity(
		Parameters.bRotationFollowsVelocity);
	ProjectileMovement->SetHomingTarget(Parameters.HomingTarget);
	ProjectileMovement->SetDirection(Parameters.Direction);
	CollisionCapsule->SetCollisionProfileName(Parameters.CollisionProfileName);
	
	GetWorldTimerManager().SetTimer(
		SelfDestroyTimerHandle,
		this,
		&AMTD_Projectile::OnSelfDestroy,
		Parameters.SecondsToSelfDestroy);
}

void AMTD_Projectile::OnBeginOverlap(
	UPrimitiveComponent *HitComponent,
	AActor *OtherActor,
	UPrimitiveComponent *OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult &SweepResult)
{
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
