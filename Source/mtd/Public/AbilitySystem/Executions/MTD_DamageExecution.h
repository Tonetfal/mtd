#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "mtd.h"

#include "MTD_DamageExecution.generated.h"

/**
 * Gameplay effect damage execution to deal damage to a target.
 *
 * Makes use of health, combat, and player sets. The player attribute set can be absent without breaking the execution.
 */
UCLASS()
class MTD_API UMTD_DamageExecution
    : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UMTD_DamageExecution();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters &ExecParams,
        FGameplayEffectCustomExecutionOutput &ExecOutput) const override;
};
