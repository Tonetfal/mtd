#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_MMC_ManaCost.generated.h"

/**
 * Gameplay modification magnitude calculation class to calculate mana cost.
 *
 * Must be used only with MTD gameplay effects or its derivatives.
 */
UCLASS()
class MTD_API UMTD_MMC_ManaCost
    : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const override;
};
