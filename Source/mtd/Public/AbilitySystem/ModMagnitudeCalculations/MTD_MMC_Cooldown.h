#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_MMC_Cooldown.generated.h"

/**
 * Gameplay modification magnitude calculation class to calculate a cooldown.
 *
 * Must be used only with MTD gameplay effects or its derivatives.
 */
UCLASS()
class MTD_API UMTD_MMC_Cooldown
    : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const override;
};
