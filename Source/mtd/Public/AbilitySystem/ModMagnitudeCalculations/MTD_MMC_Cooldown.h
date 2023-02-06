#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_MMC_Cooldown.generated.h"

/**
 * Gameplay Modification Magnitude Calculation class for UMTD_GameplayEffect class that makes use of its internal
 * CooldownDuration field.
 */
UCLASS()
class MTD_API UMTD_MMC_Cooldown : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UMTD_MMC_Cooldown();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const override;
};
