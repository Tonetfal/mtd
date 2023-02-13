#include "AbilitySystem/Effects/MTD_GameplayEffect_DamageInstant.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Executions/MTD_DamageExecution.h"

/**
 * 
 */
FGameplayEffectExecutionScopedModifierInfo GetExecScopedModInfo(const FGameplayAttribute &Attribute,
    const FGameplayTag &Tag, EGameplayModOp::Type ModOp, EGameplayEffectAttributeCaptureSource CaptureSource)
{
    // Create the execution scoped modifier info to setup and return
    FGameplayEffectExecutionScopedModifierInfo ExecScopedModInfo;
    ExecScopedModInfo.AggregatorType = EGameplayEffectScopedModifierAggregatorType::Transient;

    // Identify it with the given tag
    ExecScopedModInfo.TransientAggregatorIdentifier = Tag;

    // Capture the given attribute
    ExecScopedModInfo.CapturedAttribute.AttributeToCapture = Attribute;

    // Capture the attribute from the given source
    ExecScopedModInfo.CapturedAttribute.AttributeSource = CaptureSource;

    // Snapshot the attribute.
    // Since we're dealing with damage GE, the damage may be dealt by a sword or by a
    // projectile. In case of a projectile, if it will use the real time value (not the one had at the moment of GE
    // spec creation), then the player will be able to switch his weapon to, per say, slow and strong hammer, and the
    // projectiles will use its value instead. However, if the damage will be snapshotted at the moment when the player
    // is holding the range weapon, the projectile will remember the correct damage
    ExecScopedModInfo.CapturedAttribute.bSnapshot = true;

    // The attribute has to be set by caller, and use the given tag to identify the attribute
    FSetByCallerFloat SetByCaller;
    SetByCaller.DataTag = Tag;

    // Fill up the exec scoped info
    ExecScopedModInfo.ModifierOp = ModOp;
    ExecScopedModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

    return ExecScopedModInfo;
}

FGameplayEffectExecutionDefinition GetDamageExecutionDefinition(
    TSubclassOf<UGameplayEffectExecutionCalculation> GeExecutionCalculation)
{
    // Get MTD gameplay tags to identify each setup parameter with a tag
    const FMTD_GameplayTags Tags = FMTD_GameplayTags::Get();

    // Create an execution definition to setup and return
    FGameplayEffectExecutionDefinition ExecDef;
    
    // Use a custom execution calculation given as a parameter
    ExecDef.CalculationClass = GeExecutionCalculation;

    // Define a calculation modifier for base damage
    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetBaseDamageToUse_MetaAttribute(),
        Tags.SetByCaller_Damage_Base,
        EGameplayModOp::Override,
        EGameplayEffectAttributeCaptureSource::Target));

    // Define a calculation modifier for additive damage
    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetDamageAdditiveAttribute(),
        Tags.SetByCaller_Damage_Additive,
        EGameplayModOp::Additive,
        EGameplayEffectAttributeCaptureSource::Source));
    
    // Define a calculation modifier for damage multiplier
    ExecDef.CalculationModifiers.Add(GetExecScopedModInfo(
        UMTD_CombatSet::GetDamageMultiplierAttribute(),
        Tags.SetByCaller_Damage_Multiplier,
        EGameplayModOp::Additive,
        EGameplayEffectAttributeCaptureSource::Source));
    
    return ExecDef;
}

UMTD_GameplayEffect_DamageInstant::UMTD_GameplayEffect_DamageInstant()
{
    // Damage has to be applied instantly
    DurationPolicy = EGameplayEffectDurationType::Instant;

    // Create damage gameplay effect execution definition
    const FGameplayEffectExecutionDefinition ExecDef = GetDamageExecutionDefinition(UMTD_DamageExecution::StaticClass());

    // Add the execution to the list
    Executions.Add(ExecDef);
}
