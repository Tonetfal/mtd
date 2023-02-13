#pragma once

#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Input/MTD_InputConfig.h"
#include "mtd.h"

#include "MTD_InputComponent.generated.h"

/**
 * Custom enhanced input component to support MTD ability sets.
 *
 * @see UMTD_AbilitySet
 */
UCLASS(Config=Input)
class MTD_API UMTD_InputComponent
    : public UEnhancedInputComponent
{
    GENERATED_BODY()

public:
    /**
     * Bind a native input action to a delegate. When input action is triggered, the passed delegate will be fired
     * communicating along some input data.
     * @param   InputConfig: config containing native input actions, and input tags.
     * @param   InputTag: input tag associated with native
     * @param   TriggerEvent: trigger event the passed delegate will be fired due.
     * @param   Object: object to fire the delegate on.
     * @param   Func: delegate to fire whe ninput action triggers the passed TriggerEvent.
     */
    template <class UserClass, typename FuncType>
    void BindNativeAction(
        const UMTD_InputConfig *InputConfig,
        const FGameplayTag &InputTag,
        ETriggerEvent TriggerEvent,
        UserClass *Object,
        FuncType Func);

    /**
     * Dispatch all ability input actions from input config to delegates. When input action is triggered or completed,
     * one of the passed delegates will be fired respectively, communicating along the associated input tag.
     * @param   InputConfig: config containing all ability input actions, and input tags.
     * @param   Object: object to fire the delegates on.
     * @param   PressedFunc: delegate to fire when input action is triggered.
     * @param   ReleasedFunc: delegate to fire when input action is completed.
     */
    template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
    void BindAbilityActions(
        const UMTD_InputConfig *InputConfig,
        UserClass *Object,
        PressedFuncType PressedFunc,
        ReleasedFuncType ReleasedFunc);
};

template <class UserClass, typename FuncType>
void UMTD_InputComponent::BindNativeAction(
    const UMTD_InputConfig *InputConfig,
    const FGameplayTag &InputTag,
    ETriggerEvent TriggerEvent,
    UserClass *Object,
    FuncType Func)
{
    check(IsValid(InputConfig));

    // Find the input action associated with the given input tag
    const UInputAction *Action = InputConfig->FindNativeInputActionForTag(InputTag);
    if (!IsValid(Action))
    {
        return;
    }

    // Trigger Func on Object when Action meets TriggerEvent requirements
    BindAction(Action, TriggerEvent, Object, Func);
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UMTD_InputComponent::BindAbilityActions(
    const UMTD_InputConfig *InputConfig,
    UserClass *Object,
    PressedFuncType PressedFunc,
    ReleasedFuncType ReleasedFunc)
{
    check(IsValid(InputConfig));

    for (const FMTD_InputAction &Action : InputConfig->AbilityInputActions)
    {
        if (PressedFunc)
        {
            // Call given PressedFunc with the given InputTag when an action is triggered
            BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag);
        }

        if (ReleasedFunc)
        {
            // Call given PressedFunc with the given InputTag when an action is completed
            BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
        }
    }
}
