#include "AbilitySystem/Effects/MTD_GameplayEffect_DamageInstant.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Executions/MTD_DamageExecution.h"

FGameplayEffectExecutionScopedModifierInfo GetExecScopedModInfo(FGameplayAttribute Attribute, FGameplayTag Tag)
{
    FGameplayEffectExecutionScopedModifierInfo ExecScopedModInfo;
    ExecScopedModInfo.AggregatorType = EGameplayEffectScopedModifierAggregatorType::Transient;
    ExecScopedModInfo.TransientAggregatorIdentifier = Tag;

    ExecScopedModInfo.CapturedAttribute.AttributeToCapture = Attribute;
    ExecScopedModInfo.CapturedAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    ExecScopedModInfo.CapturedAttribute.bSnapshot = false;

    FSetByCallerFloat DmgAddSetByCaller;
    DmgAddSetByCaller.DataTag = Tag;

    ExecScopedModInfo.ModifierOp = EGameplayModOp::Additive;
    ExecScopedModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(DmgAddSetByCaller);

    return ExecScopedModInfo;
}

FGameplayEffectExecutionDefinition GetDamageExecutionDefinition(
    TSubclassOf<UGameplayEffectExecutionCalculation> GeExecutionCalculation)
{
    const FMTD_GameplayTags Tags = FMTD_GameplayTags::Get();

    FGameplayEffectExecutionDefinition ExecDef;
    ExecDef.CalculationClass = GeExecutionCalculation;

    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetDamageAdditiveAttribute(),
        Tags.SetByCaller_Damage_Additive));

    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetDamageMultiplierAttribute(),
        Tags.SetByCaller_Damage_Multiplier));

    return ExecDef;
}

UMTD_GameplayEffect_DamageInstant::UMTD_GameplayEffect_DamageInstant()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    Executions.Add(GetDamageExecutionDefinition(UMTD_DamageExecution::StaticClass()));
}

UMTD_GameplayEffect_RangedDamageInstant::UMTD_GameplayEffect_RangedDamageInstant()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    Executions.Add(GetDamageExecutionDefinition(UMTD_RangedDamageExecution::StaticClass()));
}
