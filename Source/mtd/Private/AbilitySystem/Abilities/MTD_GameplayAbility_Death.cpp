#include "AbilitySystem/Abilities/MTD_GameplayAbility_Death.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_HealthComponent.h"
#include "GameplayTagsManager.h"

UMTD_GameplayAbility_Death::UMTD_GameplayAbility_Death()
{
    UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(
        FSimpleDelegate::CreateUObject(this, &ThisClass::OnDoneAddingNativeTags));
}

void UMTD_GameplayAbility_Death::OnDoneAddingNativeTags()
{
    // Make ability fire automatically when Gameplay_Event_Death is fired
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

    // Get MTD ASC
    auto MtdAsc = CastChecked<UMTD_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

    // Cancel all abilities that aren't marked as Gameplay_Ability_Behavior_SurvivesDeath
    FGameplayTagContainer AbilityTypesToIgnore;
    AbilityTypesToIgnore.AddTag(FMTD_GameplayTags::Get().Gameplay_Ability_Behavior_SurvivesDeath);
    MtdAsc->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

    // Forbid to cancel this ability
    SetCanBeCanceled(false);

    // Start the death if allowed to do so automatically
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

    // Finish death when the ability ends
    FinishDeath();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMTD_GameplayAbility_Death::StartDeath()
{
    // Get health component
    const AActor *Actor = GetAvatarActorFromActorInfo();
    auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(Actor);
    if (!IsValid(HealthComponent))
    {
        return;
    }

    // Check whether the actor is dead
    const EMTD_DeathState DeathState = HealthComponent->GetDeathState();
    if (DeathState != EMTD_DeathState::NotDead)
    {
        return;
    }

    // Start the death
    HealthComponent->StartDeath();
}

void UMTD_GameplayAbility_Death::FinishDeath()
{
    // Get health component
    const AActor *Actor = GetAvatarActorFromActorInfo();
    auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(Actor);
    if (!IsValid(HealthComponent))
    {
        return;
    }

    // Check whether the death has already started
    const EMTD_DeathState DeathState = HealthComponent->GetDeathState();
    if (DeathState != EMTD_DeathState::DeathStarted)
    {
        return;
    }

    // Finish the death
    HealthComponent->FinishDeath();
}
