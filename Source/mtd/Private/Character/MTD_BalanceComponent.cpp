#include "Character/MTD_BalanceComponent.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "GameplayEffectExtension.h"

UMTD_BalanceComponent::UMTD_BalanceComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_BalanceComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc)
{
    const AActor *Owner = GetOwner();
    check(Owner);

    if (AbilitySystemComponent)
    {
        MTDS_ERROR("Balance component for owner [%s] has already been initilized with an ability system",
            *Owner->GetName());
        return;
    }

    AbilitySystemComponent = Asc;
    if (!AbilitySystemComponent)
    {
        MTDS_ERROR("Cannot initilize balance component for owner [%s] with a NULL ability system", *Owner->GetName());
        return;
    }

    BalanceSet = AbilitySystemComponent->GetSet<UMTD_BalanceSet>();
    if (!BalanceSet)
    {
        MTDS_ERROR("Cannot initialize balance component with NULL combat set on the ability system");
        return;
    }

    BalanceSet->OnBalanceDownDelegate.AddUObject(this, &ThisClass::OnBalanceDown);
}

void UMTD_BalanceComponent::UninitializeFromAbilitySystem()
{
    BalanceSet = nullptr;
    AbilitySystemComponent = nullptr;
}

void UMTD_BalanceComponent::OnUnregister()
{
    UninitializeFromAbilitySystem();

    Super::OnUnregister();
}

void UMTD_BalanceComponent::OnBalanceDown(
    AActor *DamageInstigator,
    AActor *DamageCauser,
    const FGameplayEffectSpec &DamageEffectSpec,
    float DamageMagnitude)
{
    if ((!IsValid(AbilitySystemComponent)) || (!IsValid(BalanceSet)))
    {
        return;
    }

    // Store all knockback related data inside UMTD_BalanceHitData
    auto HitData = NewObject<UMTD_BalanceHitData>();
    HitData->BalanceDamage = DamageMagnitude;
    HitData->KnockbackDirection = {
        BalanceSet->GetKnockbackDirectionX(),
        BalanceSet->GetKnockbackDirectionY(),
        BalanceSet->GetKnockbackDirectionZ()
    };

    // Send the "Gameplay.Event.Knockback" gameplay event through the owner's
    // ability system. This can be used to trigger a death gameplay ability.
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

    AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);

    OnBalanceDownDelegate.Broadcast(HitData);
}
