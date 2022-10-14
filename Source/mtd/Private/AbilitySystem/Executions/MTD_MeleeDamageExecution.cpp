#include "AbilitySystem/Executions/MTD_MeleeDamageExecution.h"

#include "AbilitySystemComponent.h"

UMTD_MeleeDamageExecution::UMTD_MeleeDamageExecution()
{
}

void UMTD_MeleeDamageExecution::Execute_Implementation(
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
	// float Endurance = 0.f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
	// 	MeleeDamageStatics().EnduranceDef, EvaluationParameters, Endurance);
	//
	// if (Endurance <= 1.0f)
	// {
	// 	Endurance = 1.0f;
	// }
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
