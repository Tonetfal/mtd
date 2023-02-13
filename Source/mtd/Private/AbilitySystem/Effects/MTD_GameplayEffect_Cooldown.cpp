#include "AbilitySystem/Effects/MTD_GameplayEffect_Cooldown.h"

#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_Cooldown.h"

UMTD_GameplayEffect_Cooldown::UMTD_GameplayEffect_Cooldown()
{
    // Cooldown has a duration
    DurationPolicy = EGameplayEffectDurationType::HasDuration;

    // Calculate the cooldown using a custom MMC
    FCustomCalculationBasedFloat CustomCalc;
    CustomCalc.CalculationClassMagnitude = UMTD_MMC_Cooldown::StaticClass();

    // Assign the MMC
    DurationMagnitude = FGameplayEffectModifierMagnitude(CustomCalc);
}
