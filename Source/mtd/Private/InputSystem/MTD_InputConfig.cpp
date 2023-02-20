#include "InputSystem/MTD_InputConfig.h"

static const UInputAction *FindInputActionForTag(const TArray<FMTD_InputAction> &InputActions,
    const FGameplayTag &InputTag)
{
    // Iterate though all given input actions, and return the first that matches with the input tag
    for (const FMTD_InputAction &Action : InputActions)
    {
        if (Action.InputTag.MatchesTag(InputTag))
        {
            return Action.InputAction.Get();
        }
    }

    return nullptr;
}

const UInputAction *UMTD_InputConfig::FindNativeInputActionForTag(const FGameplayTag &InputTag) const
{
    return FindInputActionForTag(NativeInputActions, InputTag);
}

const UInputAction *UMTD_InputConfig::FindAbilityInputActionForTag(const FGameplayTag &InputTag) const
{
    return FindInputActionForTag(AbilityInputActions, InputTag);
}
