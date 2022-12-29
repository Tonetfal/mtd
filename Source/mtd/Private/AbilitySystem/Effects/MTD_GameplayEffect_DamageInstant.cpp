#include "AbilitySystem/Effects/MTD_GameplayEffect_DamageInstant.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Executions/MTD_DamageExecution.h"

FGameplayEffectExecutionScopedModifierInfo GetExecScopedModInfo(const FGameplayAttribute &Attribute,
    const FGameplayTag &Tag, EGameplayModOp::Type ModOp, EGameplayEffectAttributeCaptureSource CaptureSource)
{
    FGameplayEffectExecutionScopedModifierInfo ExecScopedModInfo;
    ExecScopedModInfo.AggregatorType = EGameplayEffectScopedModifierAggregatorType::Transient;
    ExecScopedModInfo.TransientAggregatorIdentifier = Tag;

    ExecScopedModInfo.CapturedAttribute.AttributeToCapture = Attribute;
    ExecScopedModInfo.CapturedAttribute.AttributeSource = CaptureSource;
    ExecScopedModInfo.CapturedAttribute.bSnapshot = false;

    FSetByCallerFloat SetByCaller;
    SetByCaller.DataTag = Tag;

    ExecScopedModInfo.ModifierOp = ModOp;
    ExecScopedModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

    return ExecScopedModInfo;
}

FGameplayEffectExecutionDefinition GetDamageExecutionDefinition(
    TSubclassOf<UGameplayEffectExecutionCalculation> GeExecutionCalculation)
{
    const FMTD_GameplayTags Tags = FMTD_GameplayTags::Get();

    FGameplayEffectExecutionDefinition ExecDef;
    ExecDef.CalculationClass = GeExecutionCalculation;
    
    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetBaseDamageToUse_MetaAttribute(),
        Tags.SetByCaller_Damage_Base,
        EGameplayModOp::Override,
        EGameplayEffectAttributeCaptureSource::Target));

    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetDamageAdditiveAttribute(),
        Tags.SetByCaller_Damage_Additive,
        EGameplayModOp::Additive,
        EGameplayEffectAttributeCaptureSource::Source));

    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetDamageMultiplierAttribute(),
        Tags.SetByCaller_Damage_Multiplier,
        EGameplayModOp::Additive,
        EGameplayEffectAttributeCaptureSource::Source));
    
    return ExecDef;
}

UMTD_GameplayEffect_DamageInstant::UMTD_GameplayEffect_DamageInstant()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    Executions.Add(GetDamageExecutionDefinition(UMTD_DamageExecution::StaticClass()));
}
