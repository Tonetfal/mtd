#include "AbilitySystem/Effects/MTD_GameplayEffect_Cooldown.h"

#include "AbilitySystem/Executions/MTD_CooldownExecution.h"

UMTD_GameplayEffect_Cooldown::UMTD_GameplayEffect_Cooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;

    FCustomCalculationBasedFloat CustomCalc;
    CustomCalc.CalculationClassMagnitude = UMTD_CooldownExecution::StaticClass();

    DurationMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);
}
