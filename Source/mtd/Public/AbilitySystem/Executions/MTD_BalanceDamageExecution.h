#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "mtd.h"

#include "MTD_BalanceDamageExecution.generated.h"

/**
 * Gameplay effect balance damage execution that makes use of Base Balance Damage on Balance Set.
 */
UCLASS()
class MTD_API UMTD_BalanceDamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UMTD_BalanceDamageExecution();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters &ExecutionParams,
        FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};
