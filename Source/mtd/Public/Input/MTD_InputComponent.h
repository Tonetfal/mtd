#pragma once

#include "mtd.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "MTD_InputConfig.h"

#include "MTD_InputComponent.generated.h"

UCLASS(Config=Input)
class MTD_API UMTD_InputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UMTD_InputComponent();
	
public:
	template<class UserClass, typename FuncType>
	void BindNativeAction(
		const UMTD_InputConfig *InputConfig,
		const FGameplayTag &InputTag,
		ETriggerEvent TriggerEvent,
		UserClass *Object,
		FuncType Func);

	template<
		class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(
		const UMTD_InputConfig *InputConfig,
		UserClass *Object,
		PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc);
};

template<class UserClass, typename FuncType>
void UMTD_InputComponent::BindNativeAction(
	const UMTD_InputConfig *InputConfig,
	const FGameplayTag &InputTag,
	ETriggerEvent TriggerEvent,
	UserClass *Object,
	FuncType Func)
{
	check(InputConfig);

	const UInputAction *Action =
		InputConfig->FindNativeInputActionForTag(InputTag);
	if (!IsValid(Action))
		return;

	BindAction(Action, TriggerEvent, Object, Func);
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UMTD_InputComponent::BindAbilityActions(
	const UMTD_InputConfig *InputConfig,
	UserClass *Object,
	PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	for (const FMTD_InputAction &Action : InputConfig->AbilityInputActions)
	{
		if (PressedFunc)
		{
			BindAction(Action.InputAction, ETriggerEvent::Triggered, Object,
				PressedFunc, Action.InputTag);
		}
		
		if (ReleasedFunc)
		{
			BindAction(Action.InputAction, ETriggerEvent::Completed, Object,
				ReleasedFunc, Action.InputTag);
		}
	}
}
