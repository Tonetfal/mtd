#pragma once

#include "GameplayEffect.h"
#include "mtd.h"

#include "MTD_GameplayEffect_Cooldown.generated.h"

/**
 * Cooldown gameplay effect used to apply a cooldown on an ability.
 */
UCLASS()
class MTD_API UMTD_GameplayEffect_Cooldown
    : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UMTD_GameplayEffect_Cooldown();
};
