#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_TowerMaxHealth.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_BuilderSet.h"

struct FMTD_TowerHealthStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthStatDef;
	FGameplayEffectAttributeCaptureDefinition HealthStatBonusDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
	
	FMTD_TowerHealthStatics()
	{
        // Recompute the max health when any of the following attributes changes
		HealthStatDef = FGameplayEffectAttributeCaptureDefinition(
		    UMTD_BuilderSet::GetHealthStatAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		HealthStatBonusDef = FGameplayEffectAttributeCaptureDefinition(
		    UMTD_BuilderSet::GetHealthStat_BonusAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
	    MaxHealthDef = FGameplayEffectAttributeCaptureDefinition(
            UMTD_HealthSet::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static FMTD_TowerHealthStatics &TowerHealthStatics()
{
	static FMTD_TowerHealthStatics Statics;
	return Statics;
}

UMTD_MMC_TowerMaxHealth::UMTD_MMC_TowerMaxHealth()
{
	RelevantAttributesToCapture.Add(TowerHealthStatics().HealthStatDef);
	RelevantAttributesToCapture.Add(TowerHealthStatics().HealthStatBonusDef);
	RelevantAttributesToCapture.Add(TowerHealthStatics().MaxHealthDef);
}

float UMTD_MMC_TowerMaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const
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
        TowerHealthStatics().HealthStatDef, Spec, EvaluationParameters, HealthStat);
    
    float HealthStatBonus;
    GetCapturedAttributeMagnitude(
        TowerHealthStatics().HealthStatBonusDef, Spec, EvaluationParameters, HealthStatBonus);

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
