#include "AbilitySystem/Executions/MTD_DamageExecution.h"

#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition DamageStatDef;

	FDamageStatics()
	{
		HealthDef = FGameplayEffectAttributeCaptureDefinition(
			UMTD_HealthSet::GetHealthAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(
			UMTD_CombatSet::GetBaseDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			true);
		
		DamageStatDef = FGameplayEffectAttributeCaptureDefinition(
			UMTD_PlayerSet::GetDamageStatAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			true);
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
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageStatDef);
}

void UMTD_DamageExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters &ExecutionParams,
	FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
	const FGameplayEffectSpec &Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer *TargetTags =
		Spec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer *SourceTags =
		Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.TargetTags = TargetTags;
	EvaluationParams.SourceTags = SourceTags;
	
	float BaseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().BaseDamageDef, EvaluationParams, BaseDamage);
	
	float DamageStat = 0.f;
	const bool bDamageStatFound =
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().DamageStatDef, EvaluationParams, DamageStat);

	// TODO: const float DamageMultiplier = SomeSmartMathFunction(DamageStat);
	const float DamageDone = BaseDamage *
		(bDamageStatFound ? 1.f /* the math function(DamageStat) */ : 1.f);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		DamageStatics().HealthDef.AttributeToCapture,
		EGameplayModOp::Additive,
		-DamageDone));
}
