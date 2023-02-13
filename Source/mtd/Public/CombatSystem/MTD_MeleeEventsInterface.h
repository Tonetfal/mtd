#pragma once

#include "mtd.h"
#include "UObject/Interface.h"

#include "MTD_MeleeEventsInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UMTD_MeleeCombatInterface
    : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface to create a melee combat system upon.
 *
 * The intention is to have a list of hitbox entries, which consist of an unique nickname, position relatively to owner,
 * and radius (the hitboxes are all spheres).
 *
 * Animation notify states are intended to be used with animation montages in order to activate and deactivate the
 * hitboxes. An animation state only knows when a hitbox should be activated, and when it should be deactivated.
 * On notify begin it adds a hitbox nickname to the active list on the owner's combat system, and on notify end it
 * remove it from there.
 */
class MTD_API IMTD_MeleeCombatInterface
{
	GENERATED_BODY()

public:
    /**
     * Activate given melee hitboxes.
     * @param   HitboxNicknames: melee hitbox nicknames to activate.
     */
	virtual void ActivateHitboxes(const TArray<FName> &HitboxNicknames) = 0;
    
    /**
     * Deactivate present melee hitboxes.
     * @param   HitboxNicknames: melee hitbox nicknames to deactivate.
     */
	virtual void DeactivateHitboxes(const TArray<FName> &HitboxNicknames) = 0;

    /**
     * Deactive all active melee hitboxes.
     */
    virtual void DeactivateAllHitboxes() = 0;

    /**
     * Remove all hit targets from the hit list, so that they will be hit-able by this component once again.
     */
    virtual void ResetHitTargets() = 0;
};
