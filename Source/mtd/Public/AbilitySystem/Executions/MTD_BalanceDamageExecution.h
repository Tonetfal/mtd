#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "mtd.h"

#include "MTD_BalanceDamageExecution.generated.h"

/**
 * Gameplay effect balance damage execution to deal balance damage.
 *
 * Makes use of balance set.
 */
UCLASS()
class MTD_API UMTD_BalanceDamageExecution
    : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UMTD_BalanceDamageExecution();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters &ExecParams,
        FGameplayEffectCustomExecutionOutput &ExecOutput) const override;
};
