#include "Weapons/MTD_SwordWeapon.h"

#include "Components/MTD_WeaponComponent.h"
#include "Damage/MTD_BaseDamageType.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AMTD_SwordWeapon::AMTD_SwordWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	AttackArea =
		CreateDefaultSubobject<UCapsuleComponent>(
			TEXT("Attack Area Component"));
	AttackArea->SetupAttachment(Mesh);
	AttackArea->InitCapsuleSize(10.f, 15.f);
	AttackArea->SetCollisionProfileName(DeactivatedAttackCollisionProfileName);
}

void AMTD_SwordWeapon::BeginPlay()
{
	Super::BeginPlay();

	check(AttackArea);

	AttackArea->OnComponentBeginOverlap.AddDynamic(
		this, &AMTD_SwordWeapon::OnAttackAreaBeginOverlap);
}

void AMTD_SwordWeapon::Attack()
{
}

void AMTD_SwordWeapon::ForceStopAttack()
{
	AttackArea->SetCollisionProfileName(DeactivatedAttackCollisionProfileName);
}

void AMTD_SwordWeapon::OnAttackActivation()
{
	AttackArea->SetCollisionProfileName(ActivatedAttackCollisionProfileName);
}

void AMTD_SwordWeapon::OnAttackDeactivation()
{
	AttackArea->SetCollisionProfileName(DeactivatedAttackCollisionProfileName);
}

void AMTD_SwordWeapon::OnAttackAnimationTerminated()
{
}

void AMTD_SwordWeapon::OnAttackAreaBeginOverlap(
	UPrimitiveComponent *OverlappedComponent,
	AActor *OtherActor,
	UPrimitiveComponent *OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult &SweepResult)
{
	// TODO: Register every actor that we've been overlapping with since
	// attack activation up to its deactivation. Clear the list after
	// deactivation

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		nullptr,
		GetOwner(),
		UMTD_BaseDamageType::StaticClass());
}
