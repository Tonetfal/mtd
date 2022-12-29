#pragma once

#include "mtd.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect.h"
#include "MTD_GameplayEffect_DamageInstant.generated.h"

/**
 * Gameplay effect that instantly damages the given target by using its LastReceivedDamage meta attribute.
 */
UCLASS()
class MTD_API UMTD_GameplayEffect_DamageInstant : public UMTD_GameplayEffect
{
    GENERATED_BODY()

public:
    UMTD_GameplayEffect_DamageInstant();
};
