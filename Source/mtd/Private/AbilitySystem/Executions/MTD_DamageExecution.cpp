#include "AbilitySystem/Executions/MTD_DamageExecution.h"

#include "AbilitySystem/MTD_GameplayTags.h"
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
    const FMTD_GameplayTags Tags = FMTD_GameplayTags::Get();
    const FGameplayEffectSpec &Spec = ExecParams.GetOwningSpec();

    const FGameplayTagContainer *TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    const FGameplayTagContainer *SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParams;
    EvaluationParams.TargetTags = TargetTags;
    EvaluationParams.SourceTags = SourceTags;

    float DamageBase = 0.f;
    ExecParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().BaseDamage_MetaDef, EvaluationParams, DamageBase);

    const float DamageAdditive = Spec.GetSetByCallerMagnitude(Tags.SetByCaller_Damage_Additive);
    const float DamageMultiplier = Spec.GetSetByCallerMagnitude(Tags.SetByCaller_Damage_Multiplier);

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

    const float Scale = Formula(DamageStat);
    const float DamageDone = (((DamageBase * DamageMultiplier) * Scale) + DamageAdditive);

    ExecOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        UMTD_CombatSet::GetLastReceivedDamage_MetaAttribute(),
        EGameplayModOp::Override,
        DamageDone));
    
    ExecOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        UMTD_HealthSet::GetHealthAttribute(),
        EGameplayModOp::Additive,
        -DamageDone));
}
