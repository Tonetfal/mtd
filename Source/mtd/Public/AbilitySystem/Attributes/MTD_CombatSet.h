#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_CombatSet.generated.h"

/**
 * Attribute set that defines all the required data for weapons and abilities to impact a character.
 * Should be only used to communicate how much of something has to be modified on a target.
 */
UCLASS()
class MTD_API UMTD_CombatSet : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UMTD_CombatSet, DamageBase);
    ATTRIBUTE_ACCESSORS(UMTD_CombatSet, DamageRangedBase);

    ATTRIBUTE_ACCESSORS(UMTD_CombatSet, DamageAdditive);
    ATTRIBUTE_ACCESSORS(UMTD_CombatSet, DamageMultiplier);

protected:
    /** Health damage that will be added on damage computing. Must be assigned only by a Melee Weapon Instance. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageBase;

    /** Health damage that will be added on damage computing. Must be assigned only by a Ranged Weapon Instance. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageRangedBase;

    /** Health damage that will be added on damage computing. Must be assigned only by an Ability. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageAdditive;

    /** Value that will multiply health damage on damage computing. Must be assigned only by an Ability. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageMultiplier;
};
