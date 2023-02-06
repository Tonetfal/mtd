#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_MMC_TowerMaxHealth.generated.h"

/**
 * Gameplay Modification Magnitude Calculation class to calculate Tower Max Health.
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
