#include "AbilitySystem/Effects/MTD_GameplayEffect_Cooldown.h"

#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_Cooldown.h"

UMTD_GameplayEffect_Cooldown::UMTD_GameplayEffect_Cooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;

    FCustomCalculationBasedFloat CustomCalc;
    CustomCalc.CalculationClassMagnitude = UMTD_MMC_Cooldown::StaticClass();

    DurationMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);
}
