#include "AbilitySystem/Effects/MTD_GameplayEffect_Cost.h"

#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_ManaCost.h"

UMTD_GameplayEffect_Cost::UMTD_GameplayEffect_Cost()
{
    // Cost has to instantly substract the resources from owner
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModInfo;
    
    // Use mana as the resource to subtract
	ModInfo.Attribute = UMTD_ManaSet::GetManaAttribute();
	ModInfo.ModifierOp = EGameplayModOp::Additive;

    // Calculate the cost using a custom MMC
	FCustomCalculationBasedFloat CustomCalc;
	CustomCalc.CalculationClassMagnitude = UMTD_MMC_ManaCost::StaticClass();
	CustomCalc.Coefficient = -1.f;
	CustomCalc.PreMultiplyAdditiveValue = 0.f;
	CustomCalc.PostMultiplyAdditiveValue = 0.f;

    // Create a GE magnitude modifier using the custom calculation 
	const FGameplayEffectModifierMagnitude GeModMag(CustomCalc);
	ModInfo.ModifierMagnitude = GeModMag;

    // Add the created magnitude modifier
	Modifiers.Add(ModInfo);
}
