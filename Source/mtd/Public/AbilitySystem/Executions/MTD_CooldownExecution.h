#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_CooldownExecution.generated.h"

/**
 * Gameplay mod magnitude calculation class for UMTD_GameplayEffect class that
 * makes use of its internal CooldownDuration field.
 */
UCLASS()
class MTD_API UMTD_CooldownExecution : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UMTD_CooldownExecution();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const override;
};
