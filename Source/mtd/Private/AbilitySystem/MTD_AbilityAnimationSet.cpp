#include "AbilitySystem/MTD_AbilityAnimationSet.h"

void UMTD_AbilityAnimationSet::GetAbilityAnimMontages(const FGameplayTag &AbilityTag,
    FMTD_AbilityAnimations &OutAbilityAnimations) const
{
    if (!AbilityTag.IsValid())
    {
        MTDS_LOG("Ability tag is invalid.");
        return;
    }

    // Find animation montages for the given gameplay tag
    const FMTD_AbilityAnimations *Found = AbilityAnimations.Find(AbilityTag);
    if (!Found)
    {
        MTDS_WARN("No ability animation montages have been found for gameplay tag [%s]", *AbilityTag.ToString());
        return;
    }

    OutAbilityAnimations = *Found;
}
