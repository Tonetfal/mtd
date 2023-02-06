#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_ManaCost.h"

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

float UMTD_MMC_ManaCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const
{
    const UGameplayAbility *Ability = Spec.GetContext().GetAbilityInstance_NotReplicated();
    if (!IsValid(Ability))
    {
        return 0.0f;
    }

    const auto MtdAbility = CastChecked<UMTD_GameplayAbility>(Ability);
    return MtdAbility->ManaCost.GetValueAtLevel(Ability->GetAbilityLevel());
}
