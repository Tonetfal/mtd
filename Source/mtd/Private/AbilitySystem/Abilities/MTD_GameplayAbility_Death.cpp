#include "AbilitySystem/Abilities/MTD_GameplayAbility_Death.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_HealthComponent.h"
#include "GameplayTagsManager.h"

UMTD_GameplayAbility_Death::UMTD_GameplayAbility_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(
		FSimpleDelegate::CreateUObject(
			this, &ThisClass::OnDoneAddingNativeTags));
}

void UMTD_GameplayAbility_Death::OnDoneAddingNativeTags()
{
	// if (!HasAnyFlags(RF_ClassDefaultObject))
	// 	return;

	// Make ability fire automatically on GameplayEvent_Death
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FMTD_GameplayTags::Get().Gameplay_Event_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UMTD_GameplayAbility_Death::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo *ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData *TriggerEventData)
{
	check(ActorInfo);

	auto *MtdAsc = CastChecked<UMTD_AbilitySystemComponent>(
		ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(
		FMTD_GameplayTags::Get().Gameplay_Ability_Behavior_SurvivesDeath);

	MtdAsc->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	if (bAutoStartDeath)
	{
		StartDeath();
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMTD_GameplayAbility_Death::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo *ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
	check(ActorInfo);

	FinishDeath();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility,
		bWasCancelled);
}

void UMTD_GameplayAbility_Death::StartDeath()
{
	const AActor *Actor = GetAvatarActorFromActorInfo();
	auto *HealthComponent = UMTD_HealthComponent::FindHealthComponent(Actor);
	if (!IsValid(HealthComponent))
		return;

	if (HealthComponent->GetDeathState() != EMTD_DeathState::NotDead)
		return;

	HealthComponent->StartDeath();
}

void UMTD_GameplayAbility_Death::FinishDeath()
{
	const AActor *Actor = GetAvatarActorFromActorInfo();
	auto *HealthComponent = UMTD_HealthComponent::FindHealthComponent(Actor);
	if (!IsValid(HealthComponent))
		return;

	if (HealthComponent->GetDeathState() != EMTD_DeathState::DeathStarted)
		return;

	HealthComponent->FinishDeath();
}
