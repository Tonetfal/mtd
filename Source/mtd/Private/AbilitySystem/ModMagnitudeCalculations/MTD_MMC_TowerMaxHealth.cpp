#include "AbilitySystem/ModMagnitudeCalculations/MTD_MMC_TowerMaxHealth.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "Character/MTD_Tower.h"

struct FMTD_TowerHealthStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthStatDef;
	FGameplayEffectAttributeCaptureDefinition HealthStatBonusDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
	
	FMTD_TowerHealthStatics()
	{
        // Recompute the max health when any of the following attributes changes
        HealthStatDef = CAPTURE_ATTRIBUTE(UMTD_BuilderSet, HealthStat, Source, false);
        HealthStatBonusDef = CAPTURE_ATTRIBUTE(UMTD_BuilderSet, HealthStat_Bonus, Source, false);
        MaxHealthDef = CAPTURE_ATTRIBUTE(UMTD_HealthSet, MaxHealth, Source, false);
	}
};

static FMTD_TowerHealthStatics &TowerHealthStatics()
{
	static FMTD_TowerHealthStatics Statics;
	return Statics;
}

UMTD_MMC_TowerMaxHealth::UMTD_MMC_TowerMaxHealth()
{
    // List capture attributes
	RelevantAttributesToCapture.Add(TowerHealthStatics().HealthStatDef);
	RelevantAttributesToCapture.Add(TowerHealthStatics().HealthStatBonusDef);
	RelevantAttributesToCapture.Add(TowerHealthStatics().MaxHealthDef);
}

float UMTD_MMC_TowerMaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const
{
    const TArray<TWeakObjectPtr<AActor>> Actors = Spec.GetContext().GetActors();
    if (Actors.IsEmpty())
    {
        MTDS_WARN("Actors list is empty.");
        return 0.f;
    }

    check(Actors[0].IsValid());
    ensure(TSubclassOf<AMTD_Tower>(Actors[0]->GetClass())->IsChildOf(AMTD_Tower::StaticClass()));
    
    const UAbilitySystemComponent *TowerAbilitySystemComponent =
        UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actors[0].Get());
    if (!IsValid(TowerAbilitySystemComponent))
    {
        MTDS_WARN("Tower ability system component is invalid.");
        return 0.f;
    }

	const FGameplayTagContainer *SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer *TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
    
    // Get source's health stat
    float HealthStat;
    GetCapturedAttributeMagnitude(
        TowerHealthStatics().HealthStatDef, Spec, EvaluationParameters, HealthStat);
    
    // Get source's bonus health stat
    float HealthStatBonus;
    GetCapturedAttributeMagnitude(
        TowerHealthStatics().HealthStatBonusDef, Spec, EvaluationParameters, HealthStatBonus);

    // Get base max health value instead of the current one
    const float MaxHealth = TowerAbilitySystemComponent->GetNumericAttributeBase(
        UMTD_HealthSet::GetMaxHealthAttribute());

    // Sum up the health stats
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
    
    // Evaluate how much scale should be applied on health using the health stat
    const float Scale = Formula(TotalHealthStat);
    
    // Scale up the health
    const float MaxNewHealth = (MaxHealth * Scale);

    return MaxNewHealth;
}
