#include "Characters/MTD_MeleeEnemy.h"

#include "Components/MTD_WeaponComponent.h"

void AMTD_MeleeEnemy::Attack()
{
	Weapon->StartAttacking();
	Weapon->StopAttacking();
}
