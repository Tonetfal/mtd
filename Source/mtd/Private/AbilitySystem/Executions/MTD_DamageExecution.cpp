#include "AbilitySystem/Executions/MTD_DamageExecution.h"

#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"

struct FDamageStatics
{
    FGameplayEffectAttributeCaptureDefinition HealthDef;
    FGameplayEffectAttributeCaptureDefinition BaseDamage_MetaDef;
    FGameplayEffectAttributeCaptureDefinition DamageAdditiveDef;
    FGameplayEffectAttributeCaptureDefinition DamageMultiplierDef;
    FGameplayEffectAttributeCaptureDefinition DamageStatDef;

    FDamageStatics()
    {
        HealthDef = CAPTURE_ATTRIBUTE(UMTD_HealthSet, Health, Target, false);
        BaseDamage_MetaDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, BaseDamageToUse_Meta, Source, true); 
        DamageAdditiveDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageAdditive, Source, true); 
        DamageMultiplierDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageMultiplier, Source, true); 
        DamageStatDef = CAPTURE_ATTRIBUTE(UMTD_PlayerSet, DamageStat, Source, true);
    }
};

static FDamageStatics &DamageStatics()
{
    static FDamageStatics Statics;
    return Statics;
}

UMTD_DamageExecution::UMTD_DamageExecution()
{
    // List capture attributes
    RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
    RelevantAttributesToCapture.Add(DamageStatics().BaseDamage_MetaDef);
    RelevantAttributesToCapture.Add(DamageStatics().DamageAdditiveDef);
    RelevantAttributesToCapture.Add(DamageStatics().DamageMultiplierDef);
    RelevantAttributesToCapture.Add(DamageStatics().DamageStatDef);
}

void UMTD_DamageExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters &ExecParams,
    FGameplayEffectCustomExecutionOutput &ExecOutput) const
{
    const FGameplayEffectSpec &Spec = ExecParams.GetOwningSpec();

    const FGameplayTagContainer *TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    const FGameplayTagContainer *SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParams;
    EvaluationParams.TargetTags = TargetTags;
    EvaluationParams.SourceTags = SourceTags;

    // Get source's damage base
    float DamageBase = 0.f;
    ExecParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().BaseDamage_MetaDef, EvaluationParams, DamageBase);

    // Get additive damage set on the ability
    float DamageAdditive = 0.f;
    ExecParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().DamageAdditiveDef, EvaluationParams, DamageAdditive);
    
    // Get damage multiplier set on the ability
    float DamageMultiplier = 0.f;
    ExecParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().DamageMultiplierDef, EvaluationParams, DamageMultiplier);

    // Get source's damage stat
    float DamageStat = 0.f;
    ExecParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().DamageStatDef, EvaluationParams, DamageStat);

    auto Formula = [] (float T)
        {
            T = FMath::Max(T, 0.f); // Avoid negative values
            // constexpr float A = 50.f; // Scaling factor
            // constexpr float B = 1.f; // Offset. Avoid decreasing damage
            // constexpr float T0 = 1.f; // Midpoint
            // constexpr float R = 0.002f; // Growth rate
            // return ((A * (FMath::Exp(R * (T - T0)))) + B);
            return (1.f + (T / 100.f));
        };

    // Evaluate how much scale should be applied on base damage using the damage stat
    const float Scale = Formula(DamageStat);

    // Scale up the damage
    const float DamageDone = ((DamageBase * DamageMultiplier * Scale) + DamageAdditive);

    // Decrease health by damage
    ExecOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        UMTD_HealthSet::GetHealthAttribute(),
        EGameplayModOp::Additive,
        -DamageDone));
    
    // Set LastLostHealth to damage amount
    ExecOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        UMTD_HealthSet::GetLastLostHealth_MetaAttribute(),
        EGameplayModOp::Override,
        DamageDone));
}
