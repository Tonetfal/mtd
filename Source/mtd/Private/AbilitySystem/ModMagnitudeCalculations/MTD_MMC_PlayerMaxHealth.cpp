#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_PlayerMaxHealth.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"

struct FMTD_PlayerHealthStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthStatDef;
	FGameplayEffectAttributeCaptureDefinition HealthStatBonusDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
	
	FMTD_PlayerHealthStatics()
	{
        // Recompute the max health when any of the following attributes changes
		HealthStatDef = FGameplayEffectAttributeCaptureDefinition(
		    UMTD_PlayerSet::GetHealthStatAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		HealthStatBonusDef = FGameplayEffectAttributeCaptureDefinition(
		    UMTD_PlayerSet::GetHealthStat_BonusAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
	    MaxHealthDef = FGameplayEffectAttributeCaptureDefinition(
            UMTD_HealthSet::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
	}
};

static FMTD_PlayerHealthStatics &PlayerHealthStatics()
{
	static FMTD_PlayerHealthStatics Statics;
	return Statics;
}

UMTD_MMC_PlayerMaxHealth::UMTD_MMC_PlayerMaxHealth()
{
	RelevantAttributesToCapture.Add(PlayerHealthStatics().HealthStatDef);
	RelevantAttributesToCapture.Add(PlayerHealthStatics().HealthStatBonusDef);
	RelevantAttributesToCapture.Add(PlayerHealthStatics().MaxHealthDef);
}

float UMTD_MMC_PlayerMaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const
{
    const UAbilitySystemComponent *AbilitySystemComponent = Spec.GetContext().GetInstigatorAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability System Component is invalid.");
        return 0.f;
    }

	const FGameplayTagContainer *SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer *TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

    float HealthStat;
    GetCapturedAttributeMagnitude(
        PlayerHealthStatics().HealthStatDef, Spec, EvaluationParameters, HealthStat);
    
    float HealthStatBonus;
    GetCapturedAttributeMagnitude(
        PlayerHealthStatics().HealthStatBonusDef, Spec, EvaluationParameters, HealthStatBonus);

    // Get base max health value instead of the current one
    const float MaxHealth = AbilitySystemComponent->GetNumericAttributeBase(UMTD_HealthSet::GetMaxHealthAttribute());
    const float TotalHealthStat = (HealthStat + HealthStatBonus);

    auto Formula = [] (float T)
        {
            T = FMath::Max(T, 0.f); // Avoid negative values
            // constexpr float A = 50.f; // Scaling factor
            // constexpr float B = 1.f; // Offset
            // constexpr float T0 = 1.f; // Midpoint
            // constexpr float R = 0.002f; // Growth rate
            // return ((A * (FMath::Exp(R * (T - T0)))) + B);
            return (1.f + (T / 100.f));
        };
    const float Scale = Formula(TotalHealthStat);
    const float FinalValue = (MaxHealth * Scale);

    return FinalValue;
}
