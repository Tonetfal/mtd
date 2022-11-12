#include "Input/MTD_InputConfig.h"

static const UInputAction *FindInputActionForTag(const TArray<FMTD_InputAction> &InputActions, FGameplayTag InputTag)
{
    for (const FMTD_InputAction &Action :InputActions)
    {
        if (Action.InputTag.MatchesTag(InputTag))
        {
            return Action.InputAction.Get();
        }
    }

    return nullptr;
}

const UInputAction *UMTD_InputConfig::FindNativeInputActionForTag(FGameplayTag InputTag) const
{
    return FindInputActionForTag(NativeInputActions, InputTag);
}

const UInputAction *UMTD_InputConfig::FindAbilityInputActionForTag(FGameplayTag InputTag) const
{
    return FindInputActionForTag(AbilityInputActions, InputTag);
}
