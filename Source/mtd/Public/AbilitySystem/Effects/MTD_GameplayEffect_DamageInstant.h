#pragma once

#include "AbilitySystem/Effects/MTD_GameplayEffect.h"
#include "mtd.h"

#include "MTD_GameplayEffect_DamageInstant.generated.h"

/**
 * Damage gameplay effect to instantly damage a target by using its LastReceivedDamage meta attribute.
 */
UCLASS()
class MTD_API UMTD_GameplayEffect_DamageInstant
    : public UMTD_GameplayEffect
{
    GENERATED_BODY()

public:
    UMTD_GameplayEffect_DamageInstant();
};
