#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "mtd.h"

#include "MTD_MeleeDamageExecution.generated.h"

UCLASS()
class MTD_API UMTD_MeleeDamageExecution :
	public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UMTD_MeleeDamageExecution();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters &ExecutionParams,
		FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};
