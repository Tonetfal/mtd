#include "Equipment/MTD_WeaponInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"

void UMTD_WeaponInstance::ModStats(float Multiplier)
{
	Super::ModStats(Multiplier);

	UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
	if (!IsValid(Asc))
		return;

	check(Asc->GetAttributeSet(UMTD_CombatSet::StaticClass()));

	Asc->ApplyModToAttribute(UMTD_CombatSet::GetBaseDamageAttribute(),
		EGameplayModOp::Additive, WeaponStats.BaseDamage * Multiplier);
}
