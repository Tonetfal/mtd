#include "AbilitySystem/MTD_AbilitySet.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

void FMTD_AbilitySet_GrantedHandles::AddAbilitySpecHandle(
	const FGameplayAbilitySpecHandle &Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FMTD_AbilitySet_GrantedHandles::AddGameplayEffectHandle(
	const FActiveGameplayEffectHandle &Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FMTD_AbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet *Set)
{
	GrantedAttributeSets.Add(Set);
}

void FMTD_AbilitySet_GrantedHandles::TakeFromAbilitySystem(
	UMTD_AbilitySystemComponent *MtdAsc)
{
	check(MtdAsc);

	if (!MtdAsc->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets
		return;
	}

	for (const FGameplayAbilitySpecHandle &Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			MtdAsc->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle &Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			MtdAsc->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		MtdAsc->GetSpawnedAttributes_Mutable().Remove(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UMTD_AbilitySet::UMTD_AbilitySet()
{
}

void UMTD_AbilitySet::GiveToAbilitySystem(
	UMTD_AbilitySystemComponent *MtdAsc,
	FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
	UObject *SourceObject) const
{
	check(MtdAsc);

	if (!MtdAsc->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets
		return;
	}

	// Grant the gameplay abilities
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FMTD_AbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			MTDS_ERROR("Gameplay ability [%d] is invalid", AbilityIndex);
			continue;
		}

		UMTD_GameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UMTD_GameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = MtdAsc->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FMTD_AbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			MTDS_ERROR("Gameplay effect [%d] is invalid", EffectIndex);
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = MtdAsc->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, MtdAsc->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FMTD_AbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			MTDS_ERROR("Gameplay attributes [%d] are invalid", SetIndex);
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(MtdAsc->GetOwner(), SetToGrant.AttributeSet);
		MtdAsc->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}
