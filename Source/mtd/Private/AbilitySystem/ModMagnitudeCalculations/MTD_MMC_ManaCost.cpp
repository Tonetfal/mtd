#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_ManaCost.h"

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

float UMTD_MMC_ManaCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const
{
    const FGameplayEffectContextHandle &EffectContext = Spec.GetContext();
    const UGameplayAbility *Ability = EffectContext.GetAbilityInstance_NotReplicated();
    if (!IsValid(Ability))
    {
        return 0.f;
    }
    
    const auto MtdAbility = Cast<UMTD_GameplayAbility>(Ability);
    if (!IsValid(MtdAbility))
    {
        MTDS_WARN("Failed to cast gameplay ability to MTD gameplay ability.");
        return 0.f;
    }
    
    // Calculate cooldown duration using the ability level
    const int32 AbilityLevel = Ability->GetAbilityLevel();
    const float ManaCost = MtdAbility->ManaCost.GetValueAtLevel(AbilityLevel);
    
    return ManaCost;
}
