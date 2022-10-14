#include "AbilitySystem/Effects/MTD_GameplayEffect_Attack.h"

UMTD_GameplayEffect_Attack::UMTD_GameplayEffect_Attack()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude =
		FGameplayEffectModifierMagnitude(FSetByCallerFloat());
}
