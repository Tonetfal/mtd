#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "mtd.h"

#include "MTD_DamageExecution.generated.h"

UCLASS()
class MTD_API UMTD_DamageExecution :
	public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UMTD_DamageExecution();
	
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters &ExecutionParams,
		FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};
