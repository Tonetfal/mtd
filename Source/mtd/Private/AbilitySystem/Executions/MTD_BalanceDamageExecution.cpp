#include "AbilitySystem/Executions/MTD_BalanceDamageExecution.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Attributes/MTD_BalanceSet.h"

struct FBalanceStatics
{
    FGameplayEffectAttributeCaptureDefinition TargetLastReceivedDamage;
    FGameplayEffectAttributeCaptureDefinition SourceBalanceDamage;

    FBalanceStatics()
    {
        TargetLastReceivedDamage = FGameplayEffectAttributeCaptureDefinition(
            UMTD_BalanceSet::GetLastReceivedDamageAttribute(),
            EGameplayEffectAttributeCaptureSource::Target,
            false);
        
        SourceBalanceDamage = FGameplayEffectAttributeCaptureDefinition(
            UMTD_BalanceSet::GetBaseDamageAttribute(),
            EGameplayEffectAttributeCaptureSource::Source,
            true);
    }
};

static FBalanceStatics &BalanceStatics()
{
    static FBalanceStatics Statics;
    return Statics;
}

UMTD_BalanceDamageExecution::UMTD_BalanceDamageExecution()
{
    RelevantAttributesToCapture.Add(BalanceStatics().TargetLastReceivedDamage);
    RelevantAttributesToCapture.Add(BalanceStatics().SourceBalanceDamage);
}

void UMTD_BalanceDamageExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters &ExecutionParams,
    FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
    const FMTD_GameplayTags Tags = FMTD_GameplayTags::Get();
    const FGameplayEffectSpec &Spec = ExecutionParams.GetOwningSpec();

    const FGameplayTagContainer *TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    const FGameplayTagContainer *SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParams;
    EvaluationParams.TargetTags = TargetTags;
    EvaluationParams.SourceTags = SourceTags;

    float Damage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        BalanceStatics().SourceBalanceDamage, EvaluationParams, Damage);

   OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        BalanceStatics().TargetLastReceivedDamage.AttributeToCapture,
        EGameplayModOp::Override,
        Damage));
}
