#include "Weapons/MTD_BaseWeapon.h"

AMTD_BaseWeapon::AMTD_BaseWeapon()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName("NoCollision");
}

void AMTD_BaseWeapon::Attack()
{
}

void AMTD_BaseWeapon::ForceStopAttack()
{
}

void AMTD_BaseWeapon::OnAttackActivation()
{
}

void AMTD_BaseWeapon::OnAttackDeactivation()
{
}

void AMTD_BaseWeapon::OnAttackAnimationTerminated()
{
}

void AMTD_BaseWeapon::SetAttackCollisionProfileName(FName ProfileName)
{
	ActivatedAttackCollisionProfileName = ProfileName;
}

