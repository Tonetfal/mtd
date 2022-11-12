#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "mtd.h"

#include "MTD_DamageExecution.generated.h"

/**
 * Gameplay effect damage execution that makes use of Base Damage on Combat Set.
 */
UCLASS()
class MTD_API UMTD_DamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UMTD_DamageExecution();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters &ExecutionParams,
        FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};

/**
 * Gameplay effect damage execution that makes use of Damage Ranged Base on Combat Set.
 */
UCLASS()
class MTD_API UMTD_RangedDamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UMTD_RangedDamageExecution();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters &ExecutionParams,
        FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};
