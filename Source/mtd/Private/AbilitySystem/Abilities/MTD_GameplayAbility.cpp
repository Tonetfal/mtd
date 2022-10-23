#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect_Cooldown.h"
#include "Character/MTD_BaseCharacter.h"

UMTD_GameplayAbility::UMTD_GameplayAbility()
{
	CooldownGameplayEffectClass = UMTD_GameplayEffect_Cooldown::StaticClass();
}

void UMTD_GameplayAbility::OnGiveAbility(
	const FGameplayAbilityActorInfo *ActorInfo,
	const FGameplayAbilitySpec &Spec) 
{
	Super::OnGiveAbility(ActorInfo,Spec);

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

const UAnimMontage *UMTD_GameplayAbility::GetRandomAbilityAnimMontage(
	AActor *AvatarActor) const
{
	if (!IsValid(AvatarActor))
		return nullptr;

	const auto Character = CastChecked<AMTD_BaseCharacter>(AvatarActor);
	TArray<UAnimMontage*> Animations =
		Character->GetAbilityAnimMontages(MainAbilityTag).Animations;

	const int32 Size = Animations.Num();
	if (Size == 0)
	{
		MTDS_WARN("There are no animations to play with Gameplay Tag [%s]",
			*MainAbilityTag.ToString());
		return nullptr;
	}

	const int32 Index = FMath::Rand() % Size;
	return Animations[Index];
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

void UMTD_GameplayAbility::TryActivateAbilityOnSpawn(
	const FGameplayAbilityActorInfo *ActorInfo,
	const FGameplayAbilitySpec &Spec) const
{
	check(ActorInfo);

	UAbilitySystemComponent *Asc = ActorInfo->AbilitySystemComponent.Get();
	
	if (Spec.IsActive() ||
		ActivationPolicy != EMTD_AbilityActivationPolicy::OnSpawn ||
		!Asc)
	{
		return;
	}

	Asc->TryActivateAbility(Spec.Handle);
}
