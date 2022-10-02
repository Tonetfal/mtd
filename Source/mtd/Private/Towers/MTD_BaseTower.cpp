#include "Towers/MTD_BaseTower.h"

#include "Controllers/MTD_TowerController.h"
#include "Components/MTD_HealthComponent.h"
#include "Projectiles/MTD_Projectile.h"
#include "Utilities/MTD_Utilities.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AMTD_BaseTower::AMTD_BaseTower()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionCapsule =
		CreateDefaultSubobject<UCapsuleComponent>(
			TEXT("Collision Capsule Component"));
	SetRootComponent(CollisionCapsule);
	CollisionCapsule->InitCapsuleSize(35.f, 80.f);
	CollisionCapsule->SetCollisionProfileName(AllyCollisionProfileName);

	Mesh =
		CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionProfileName("NoCollision");

	Health =
		CreateDefaultSubobject<UMTD_HealthComponent>(
			TEXT("MTD Health Component"));
	
	SpawnPosition =
		CreateDefaultSubobject<USphereComponent>(TEXT("Spawn Position"));
	SpawnPosition->SetupAttachment(GetRootComponent());
	SpawnPosition->InitSphereRadius(25.f);
	SpawnPosition->SetCollisionProfileName(FName("NoCollision"));
	
	bCanAffectNavigationGeneration = false;
}

void AMTD_BaseTower::BeginPlay()
{
	Super::BeginPlay();

	check(SpawnPosition);
	check(Health);

	if (IsValid(Controller))
	{
		if (!IsValid(Cast<AMTD_TowerController>(Controller)))
		{
			MTD_WARN("Failed to cast %s to AMTD_TowerController", 
				*Controller->GetName());
		}
	}
	else
	{
		MTD_WARN("Controller is invalid");
	}
	
	if (!ProjectileClass)
	{
		MTD_WARN("Projectile class is not set");
	}

	Health->OnDeathDelegate.AddUObject(
		this, &AMTD_BaseTower::OnDestroy);
	
	SetupVisionParameters();
}

void AMTD_BaseTower::SetupVisionParameters()
{
	auto *TowerController =
		FMTD_Utilities::GetPawnController<AMTD_TowerController>(this);
	if (!IsValid(TowerController))
		return;

	TowerController->SetVisionRange(GetScaledVisionRange());
	TowerController->SetPeripheralVisionHalfAngleDegrees(
		GetScaledVisionHalfDegrees());
}

void AMTD_BaseTower::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsOnReload)
	{
		auto *TowerController =
			FMTD_Utilities::GetPawnController<AMTD_TowerController>(this);
		if (!IsValid(TowerController))
			return;

		OnFire(TowerController->GetFireTarget());
	}
}

void AMTD_BaseTower::OnFire(AActor *FireTarget)
{
	if (!ProjectileClass || !IsValid(FireTarget))
		return;

	AMTD_Projectile *Projectile = SpawnProjectile();
	if (!IsValid(Projectile))
	{
		MTD_WARN("%s has failed to spawn a projectile", *GetName());
		return;
	}

	ProjectileSetup(Projectile, FireTarget);

	// Has to be moved to child classes instead
	bIsOnReload = true;
	FTimerHandle ReloadTimerHandle;
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this,
		&AMTD_BaseTower::OnReload, 60.f / GetScaledFirerate());
}

void AMTD_BaseTower::OnReload()
{
	bIsOnReload = false;
}

void AMTD_BaseTower::OnDestroy_Implementation()
{
	// Play death animation
	
	Destroy();
}

void AMTD_BaseTower::ProjectileSetup(
	AMTD_Projectile *Projectile, AActor *FireTarget)
{
	if (!IsValid(FireTarget))
	{
		MTD_WARN("Fire target is invalid");
		return;
	}
	
	if (!IsValid(Projectile))
	{
		MTD_WARN("Projectile is invalid");
		return;
	}
	
	FMTD_ProjectileParameters Params;

	float Len;
	Params.InitialSpeed = GetScaledInitialSpeed();
	Params.MaxSpeed = GetScaledProjectileMaxSpeed();
	Params.Acceleration = GetScaledAcceleration();
	Params.Damage = GetScaledDamage();
	Params.bIsRadial = bAreProjectilesRadial;
	Params.bRotationFollowsVelocity = true;
	Params.HomingTarget =
		bAreProjectilesHoming ? FireTarget->GetRootComponent() : nullptr;
	Params.CollisionProfileName = AllyProjectileCollisionProfileName;
	
	FVector(FireTarget->GetActorLocation() -
		SpawnPosition->GetComponentLocation()).ToDirectionAndLength(
			Params.Direction, Len);
	Projectile->SetupParameters(Params);
}

AMTD_Projectile *AMTD_BaseTower::SpawnProjectile()
{
	if (!ProjectileClass || !GetWorld())
		return nullptr;

	const FVector Location = SpawnPosition->GetComponentLocation();
	const FRotator Rotation = GetActorRotation();
	FActorSpawnParameters SpawnParms;
	
	SpawnParms.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AMTD_Projectile *Projectile = Cast<AMTD_Projectile>(GetWorld()->SpawnActor(
		ProjectileClass, &Location, &Rotation, SpawnParms));
	Projectile->SetOwner(this);
	
	return Projectile;
}

float AMTD_BaseTower::GetScaledInitialSpeed() const
{
	return BaseProjectileInitialSpeed;
}

float AMTD_BaseTower::GetScaledFirerate() const
{
	return BaseFirerate;
}

float AMTD_BaseTower::GetScaledAcceleration() const
{
	return BaseProjectileAcceleration;
}

float AMTD_BaseTower::GetScaledVisionRange() const
{
	return BaseVisionRange;
}

float AMTD_BaseTower::GetScaledVisionHalfDegrees() const
{
	return BaseVisionHalfDegrees;
}

int32 AMTD_BaseTower::GetScaledDamage() const
{
	return BaseDamage;
}

float AMTD_BaseTower::GetScaledProjectileMaxSpeed() const
{
	return BaseProjectileMaxSpeed;
}
