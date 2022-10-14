#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

#include "AbilitySystem/Effects/MTD_GameplayEffect_Cooldown.h"

UMTD_GameplayAbility::UMTD_GameplayAbility()
{
	CooldownGameplayEffectClass = UMTD_GameplayEffect_Cooldown::StaticClass();
}

const FGameplayTagContainer *UMTD_GameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer *MutableTags =
		const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset();

	const FGameplayTagContainer *ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);

	return MutableTags;
}

void UMTD_GameplayAbility::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect *CooldownGe = GetCooldownGameplayEffect();
	if (!CooldownGe)
		return;

	FGameplayEffectSpecHandle GeSpecHandle = MakeOutgoingGameplayEffectSpec(
		CooldownGe->GetClass(), GetAbilityLevel());

	GeSpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);

	// ReSharper disable once CppExpressionWithoutSideEffects
	ApplyGameplayEffectSpecToOwner(
		Handle, ActorInfo, ActivationInfo, GeSpecHandle);
}
