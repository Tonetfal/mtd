#include "AbilitySystem/Effects/MTD_GameplayEffect_Cost.h"

#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_ManaCost.h"

UMTD_GameplayEffect_Cost::UMTD_GameplayEffect_Cost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModInfo;
	
	ModInfo.Attribute = UMTD_ManaSet::GetManaAttribute();
	ModInfo.ModifierOp = EGameplayModOp::Additive;
	
	FCustomCalculationBasedFloat CustomCalc;
	CustomCalc.CalculationClassMagnitude = UMTD_MMC_ManaCost::StaticClass();
	CustomCalc.Coefficient = -1.f;
	CustomCalc.PreMultiplyAdditiveValue = 0.f;
	CustomCalc.PostMultiplyAdditiveValue = 0.f;
	
	FGameplayEffectModifierMagnitude GeModMag(CustomCalc);
	ModInfo.ModifierMagnitude = GeModMag;

	Modifiers.Add(ModInfo);
}
