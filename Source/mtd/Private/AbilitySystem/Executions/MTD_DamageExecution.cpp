#include "AbilitySystem/Executions/MTD_DamageExecution.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"

struct FDamageStatics
{
    FGameplayEffectAttributeCaptureDefinition HealthDef;
    FGameplayEffectAttributeCaptureDefinition DamageBaseDef;
    FGameplayEffectAttributeCaptureDefinition DamageAdditiveDef;
    FGameplayEffectAttributeCaptureDefinition DamageMultiplierDef;
    FGameplayEffectAttributeCaptureDefinition DamageStatDef;

    FDamageStatics()
    {
        HealthDef = CAPTURE_ATTRIBUTE(UMTD_HealthSet, Health, Target, false); 
        DamageBaseDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageBase, Source, true); 
        DamageAdditiveDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageAdditive, Source, true); 
        DamageMultiplierDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageMultiplier, Source, true); 
        DamageStatDef = CAPTURE_ATTRIBUTE(UMTD_PlayerSet, DamageStat, Source, true);
    }
};

struct FRangedDamageStatics
{
    FGameplayEffectAttributeCaptureDefinition HealthDef;
    FGameplayEffectAttributeCaptureDefinition DamageRangedBaseDef;
    FGameplayEffectAttributeCaptureDefinition DamageAdditiveDef;
    FGameplayEffectAttributeCaptureDefinition DamageMultiplierDef;
    FGameplayEffectAttributeCaptureDefinition DamageStatDef;

    FRangedDamageStatics()
    {
        HealthDef = CAPTURE_ATTRIBUTE(UMTD_HealthSet, Health, Target, false);
        DamageRangedBaseDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageRangedBase, Source, true);
        DamageAdditiveDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageAdditive, Source, true);
        DamageMultiplierDef = CAPTURE_ATTRIBUTE(UMTD_CombatSet, DamageMultiplier, Source, true);
        DamageStatDef = CAPTURE_ATTRIBUTE(UMTD_PlayerSet, DamageStat, Source, true);
    }
};

static FDamageStatics &BalanceDamageStatics()
{
    static FDamageStatics Statics;
    return Statics;
}

static FRangedDamageStatics &RangedDamageStatics()
{
    static FRangedDamageStatics Statics;
    return Statics;
}

UMTD_DamageExecution::UMTD_DamageExecution()
{
    RelevantAttributesToCapture.Add(BalanceDamageStatics().HealthDef);
    RelevantAttributesToCapture.Add(BalanceDamageStatics().DamageBaseDef);
    RelevantAttributesToCapture.Add(BalanceDamageStatics().DamageAdditiveDef);
    RelevantAttributesToCapture.Add(BalanceDamageStatics().DamageMultiplierDef);
    RelevantAttributesToCapture.Add(BalanceDamageStatics().DamageStatDef);
}

static void Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters &ExecutionParams,
    FGameplayEffectCustomExecutionOutput &ExecOutput,
    FGameplayEffectAttributeCaptureDefinition DamageDef);

void UMTD_DamageExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters &ExecutionParams,
    FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
    ::Execute_Implementation(ExecutionParams, OutExecutionOutput, BalanceDamageStatics().DamageBaseDef);
}

UMTD_RangedDamageExecution::UMTD_RangedDamageExecution()
{
    RelevantAttributesToCapture.Add(RangedDamageStatics().HealthDef);
    RelevantAttributesToCapture.Add(RangedDamageStatics().DamageRangedBaseDef);
    RelevantAttributesToCapture.Add(RangedDamageStatics().DamageAdditiveDef);
    RelevantAttributesToCapture.Add(RangedDamageStatics().DamageMultiplierDef);
    RelevantAttributesToCapture.Add(RangedDamageStatics().DamageStatDef);
}

void UMTD_RangedDamageExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters &ExecutionParams,
    FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
    ::Execute_Implementation(ExecutionParams, OutExecutionOutput, RangedDamageStatics().DamageRangedBaseDef);
}

static void Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters &ExecutionParams,
    FGameplayEffectCustomExecutionOutput &ExecOutput,
    FGameplayEffectAttributeCaptureDefinition DamageDef)
{
    const FMTD_GameplayTags Tags = FMTD_GameplayTags::Get();
    const FGameplayEffectSpec &Spec = ExecutionParams.GetOwningSpec();

    const FGameplayTagContainer *TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    const FGameplayTagContainer *SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParams;
    EvaluationParams.TargetTags = TargetTags;
    EvaluationParams.SourceTags = SourceTags;

    float DamageBase = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageDef, EvaluationParams, DamageBase);

    float DamageAdditive = Spec.GetSetByCallerMagnitude(Tags.SetByCaller_Damage_Additive);
    float DamageMultiplier = Spec.GetSetByCallerMagnitude(Tags.SetByCaller_Damage_Multiplier);

    float DamageStat = 0.f;
    const bool bDamageStatFound = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        BalanceDamageStatics().DamageStatDef, EvaluationParams, DamageStat);

    // TODO: const float DamageMultiplier = SomeSmartMathFunction(DamageStat);
    const float DamageDone =
        (DamageBase + DamageAdditive) * DamageMultiplier *
        (bDamageStatFound ? 1.f /* the math function(DamageStat) */ : 1.f);

    ExecOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        BalanceDamageStatics().HealthDef.AttributeToCapture,
        EGameplayModOp::Additive,
        -DamageDone));
}
