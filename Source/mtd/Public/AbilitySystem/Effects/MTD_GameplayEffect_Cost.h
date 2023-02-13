#pragma once

#include "AbilitySystem/Effects/MTD_GameplayEffect.h"
#include "mtd.h"

#include "MTD_GameplayEffect_Cost.generated.h"

/**
 * Cost gameplay effect used to give a cost to an ability.
 */
UCLASS()
class MTD_API UMTD_GameplayEffect_Cost
    : public UMTD_GameplayEffect
{
	GENERATED_BODY()

public:
	UMTD_GameplayEffect_Cost();
};
