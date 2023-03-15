#include "Character/Components/MTD_BalanceComponent.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "GameplayEffectExtension.h"

UMTD_BalanceComponent::UMTD_BalanceComponent()
{
    // Nothing to tick for
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_BalanceComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent)
{
    Super::InitializeWithAbilitySystem(InAbilitySystemComponent);
    if (!AbilitySystemComponent)
    {
        return;
    }

    // Cache the balance set to avoid searching for it in ability system component every time it's needed
    BalanceSet = AbilitySystemComponent->GetSet<UMTD_BalanceSet>();
    if (!IsValid(BalanceSet))
    {
        MTDS_ERROR("Cannot initialize balance component with NULL balance set on the ability system.");
        return;
    }

    // Listen for balance down events
    BalanceSet->OnBalanceDownDelegate.AddUObject(this, &ThisClass::OnBalanceDown);
}

void UMTD_BalanceComponent::UninitializeFromAbilitySystem()
{
    // Nullify ability system related data
    BalanceSet = nullptr;
    
    Super::UninitializeFromAbilitySystem();
}

void UMTD_BalanceComponent::OnBalanceDown(
    AActor *DamageInstigator,
    AActor *DamageCauser,
    const FGameplayEffectSpec &DamageEffectSpec,
    float DamageMagnitude)
{
    if (((!IsValid(AbilitySystemComponent)) || (!IsValid(BalanceSet))))
    {
        return;
    }

    // Store all knockback related data inside UMTD_BalanceHitData
    auto HitData = NewObject<UMTD_BalanceHitData>();
    HitData->BalanceDamage = DamageMagnitude;
    HitData->KnockBackDirection = {
        BalanceSet->GetKnockbackDirectionX_Meta(),
        BalanceSet->GetKnockbackDirectionY_Meta(),
        BalanceSet->GetKnockbackDirectionZ_Meta()
    };

    // Send the "Gameplay.Event.Knockback" gameplay event through the owner's ability system.
    // This can be used to trigger a death gameplay ability.
    FGameplayEventData Payload;
    Payload.EventTag = FMTD_GameplayTags::Get().Gameplay_Event_Knockback;
    Payload.Instigator = DamageInstigator;
    Payload.Target = AbilitySystemComponent->GetAvatarActor();
    Payload.OptionalObject = DamageEffectSpec.Def;
    Payload.OptionalObject2 = HitData;
    Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
    Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
    Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
    Payload.EventMagnitude = DamageMagnitude;

    // Send a gameplay event containing different balance related data
    AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);

    // Notify about balance down
    OnBalanceDownDelegate.Broadcast(HitData);
}
