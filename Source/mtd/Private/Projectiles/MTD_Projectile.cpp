#include "Projectiles/MTD_Projectile.h"

#include "Projectiles/MTD_ProjectileMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AMTD_Projectile::AMTD_Projectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(
		TEXT("Collision Capsule Component"));
	SetRootComponent(CollisionCapsule);
	CollisionCapsule->InitCapsuleSize(50.f, 50.f);
	CollisionCapsule->SetCollisionProfileName("NoCollision");
	CollisionCapsule->SetSimulatePhysics(false);
	CollisionCapsule->SetEnableGravity(false);
	
	Mesh =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionProfileName(FName("NoCollision"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);

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

	GetWorldTimerManager().SetTimer(
		SelfDestroyTimerHandle,
		this,
		&AMTD_Projectile::OnDestroy,
		30.f);
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
}

void AMTD_Projectile::OnBeginOverlap(
	UPrimitiveComponent *HitComponent,
	AActor *OtherActor,
	UPrimitiveComponent *OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult &SweepResult)
{
	if (Parameters.bIsRadial)
	{
		const UWorld *World = GetWorld();
		if (!IsValid(World))
		{
			MTD_WARN("World is invalid");
			return;
		}

		TArray<AActor*> IgnoreActors; // add all allies, ie player and towers, if we are an ally proj
		UGameplayStatics::ApplyRadialDamage(
			World,
			Parameters.Damage,
			SweepResult.Location, // or GetActorLocation()
			Parameters.RadialDamageRadius,
			Parameters.DamageTypeClass,
			IgnoreActors,
			this,
			nullptr, // tower reference
			false);
		
	}
	else
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			Parameters.Damage,
			nullptr, // tower reference
			GetOwner(), 
			Parameters.DamageTypeClass);
	}
	OnDestroy();
}

void AMTD_Projectile::OnDestroy()
{
	if (SelfDestroyTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(SelfDestroyTimerHandle);
	}

	// Play VFX
	// ...

	Destroy();
}
