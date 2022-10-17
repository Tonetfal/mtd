#include "AbilitySystem/Executions/MTD_DamageExecution.h"

#include "AbilitySystemComponent.h"

UMTD_DamageExecution::UMTD_DamageExecution()
{
}

void UMTD_DamageExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters &ExecutionParams,
	FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
	UAbilitySystemComponent *TargetAsc =
		ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent *SourceAsc =
		ExecutionParams.GetSourceAbilitySystemComponent();

	AActor *TargetActor =
		IsValid(TargetAsc) ? TargetAsc->GetAvatarActor() : nullptr;
	AActor *SourceActor =
		IsValid(SourceAsc) ? SourceAsc->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec &Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer *TargetTags =
		Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer *SourceTags =
		Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalutionParams;
	EvalutionParams.TargetTags = TargetTags;
	EvalutionParams.SourceTags = SourceTags;
	
	// float BaseDamage = 0.f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
	// 	MeleeDamageStatics().WeaponDamageDef, EvaluationParameters, BaseDamage);
	//
	// float Strength = 0.f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
	// 	MeleeDamageStatics().StrengthDef, EvaluationParameters, Strength);
	//
	// float DamageDone = BaseDamage + (Strength / Endurance);
	//
	// if (DamageDone <= 0.f)
	// {
	// 	DamageDone = 1.0f;
	// }
	//
	// OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
	// 	MeleeDamageStatics().HealthProperty,
	// 	EGameplayModOp::Additive,
	// 	-DamageDone));
}
