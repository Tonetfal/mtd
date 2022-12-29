#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "mtd.h"

#include "MTD_DamageExecution.generated.h"

/**
 * Gameplay effect damage execution to compute the amount damage to deal to target, and store it inside
 * LastReceivedDamage meta attribute.
 */
UCLASS()
class MTD_API UMTD_DamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UMTD_DamageExecution();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters &ExecParams,
        FGameplayEffectCustomExecutionOutput &ExecOutput) const override;
};
