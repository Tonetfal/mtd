#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_Cooldown.h"

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "AbilitySystemComponent.h"

UMTD_MMC_Cooldown::UMTD_MMC_Cooldown()
{
}

float UMTD_MMC_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const
{
    const auto Ability = Cast<UMTD_GameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());
    if (!IsValid(Ability))
    {
        return 0.f;
    }

    return Ability->CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());
}
