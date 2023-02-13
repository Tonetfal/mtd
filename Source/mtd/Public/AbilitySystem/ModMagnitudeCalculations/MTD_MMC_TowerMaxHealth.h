#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_MMC_TowerMaxHealth.generated.h"

/**
 * Gameplay modification magnitude calculation class to calculate tower max health.
 *
 * Makes use of health and builder sets.
 */
UCLASS()
class MTD_API UMTD_MMC_TowerMaxHealth
    : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UMTD_MMC_TowerMaxHealth();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const override;
};
