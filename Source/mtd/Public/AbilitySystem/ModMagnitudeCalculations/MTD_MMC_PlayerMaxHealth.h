#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_MMC_PlayerMaxHealth.generated.h"

/**
 * Gameplay Modification Magnitude Calculation class to calculate Player Max Health.
 */
UCLASS()
class MTD_API UMTD_MMC_PlayerMaxHealth
    : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UMTD_MMC_PlayerMaxHealth();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const override;
};
