#pragma once

#include "mtd.h"
#include "GameplayEffect.h"
#include "MTD_GameplayEffect_DamageInstant.generated.h"

/**
 * Gameplay effect that instantly damages the given target making use of Base Damage on Combat Set.
 */
UCLASS()
class MTD_API UMTD_GameplayEffect_DamageInstant : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UMTD_GameplayEffect_DamageInstant();
};

/**
 * Gameplay effect that instantly damages the given target making use of Base Ranged Damage on Combat Set.
 */
UCLASS()
class MTD_API UMTD_GameplayEffect_RangedDamageInstant : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UMTD_GameplayEffect_RangedDamageInstant();
};
