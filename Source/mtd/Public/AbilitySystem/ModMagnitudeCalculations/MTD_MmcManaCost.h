#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "mtd.h"

#include "MTD_MmcManaCost.generated.h"

UCLASS()
class MTD_API UMTD_MmcManaCost : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec &Spec) const override;
};
