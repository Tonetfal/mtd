#pragma once

#include "GameplayEffect.h"
#include "mtd.h"

#include "MTD_GameplayEffect_Attack.generated.h"

/**
 * Attack gameplay effect that persists on an actor while it attacks, and some time after in order to be able to combo
 * attacks.
 *
 * If some ability makes use of this GE, then it has to create a new one. Whenever that ability will be triggered, it
 * will check whether the GE is already active, and change its behavior depending on GE level, or activate the GE if it
 * isn't.
 */
UCLASS()
class MTD_API UMTD_GameplayEffect_Attack
    : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UMTD_GameplayEffect_Attack();
};
