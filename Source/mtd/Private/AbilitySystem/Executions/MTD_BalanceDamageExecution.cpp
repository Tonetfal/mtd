#include "AbilitySystem/Executions/MTD_BalanceDamageExecution.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Attributes/MTD_BalanceSet.h"

struct FBalanceStatics
{
    FGameplayEffectAttributeCaptureDefinition TargetLastReceivedDamage_MetaDef;
    FGameplayEffectAttributeCaptureDefinition TargetResistDef;
    FGameplayEffectAttributeCaptureDefinition SourceBalanceDamageDef;

    FBalanceStatics()
    {
        TargetLastReceivedDamage_MetaDef = CAPTURE_ATTRIBUTE(UMTD_BalanceSet, LastReceivedBalanceDamage_Meta, Target, false);
        TargetResistDef = CAPTURE_ATTRIBUTE(UMTD_BalanceSet, Resist, Target, true);
        SourceBalanceDamageDef = CAPTURE_ATTRIBUTE(UMTD_BalanceSet, Damage, Source, true);
    }
};

static FBalanceStatics &BalanceStatics()
{
    static FBalanceStatics Statics;
    return Statics;
}

UMTD_BalanceDamageExecution::UMTD_BalanceDamageExecution()
{
    RelevantAttributesToCapture.Add(BalanceStatics().TargetLastReceivedDamage_MetaDef);
    RelevantAttributesToCapture.Add(BalanceStatics().TargetResistDef);
    RelevantAttributesToCapture.Add(BalanceStatics().SourceBalanceDamageDef);
}

void UMTD_BalanceDamageExecution::Execute_Implementation(
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

    float Damage = 0.f;
    ExecParams.AttemptCalculateCapturedAttributeMagnitude(
        BalanceStatics().SourceBalanceDamageDef, EvaluationParams, Damage);

    float Resist = 0.f;
    ExecParams.AttemptCalculateCapturedAttributeMagnitude(
        BalanceStatics().TargetResistDef, EvaluationParams, Resist);

    const float IgnoredDamage = (Damage / 100.f) * Resist;
    Damage -= IgnoredDamage;
    
    ExecOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
         UMTD_BalanceSet::GetLastReceivedBalanceDamage_MetaAttribute(),
         EGameplayModOp::Override,
         Damage));
}
