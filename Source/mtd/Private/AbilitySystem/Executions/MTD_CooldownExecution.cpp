#include "AbilitySystem/Executions/MTD_CooldownExecution.h"

#include "AbilitySystemComponent.h"

UMTD_CooldownExecution::UMTD_CooldownExecution()
{
}

float UMTD_CooldownExecution::CalculateBaseMagnitude_Implementation(
	const FGameplayEffectSpec &Spec) const
{
	const auto Ability = Cast<UMTD_GameplayAbility>(
		Spec.GetContext().GetAbilityInstance_NotReplicated());
	if (!IsValid(Ability))
		return 0.f;

	return
		Ability->CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());
}
