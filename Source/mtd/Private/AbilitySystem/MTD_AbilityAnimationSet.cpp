#include "AbilitySystem/MTD_AbilityAnimationSet.h"

FMTD_AbilityAnimations UMTD_AbilityAnimationSet::GetAbilityAnimMontages(FGameplayTag AbilityTag) const
{
    if (!AbilityTag.IsValid())
    {
        return {};
    }

    const FMTD_AbilityAnimations *Found = AbilityAnimations.Find(AbilityTag);
    return Found ? *Found : FMTD_AbilityAnimations();
}
