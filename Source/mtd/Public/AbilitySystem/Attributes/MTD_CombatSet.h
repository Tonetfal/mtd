#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_CombatSet.generated.h"

/**
 * Attribute set that defines all the required data for weapons and abilities to deal damage.
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
    /// A constant value of damage that will be added on damage computing. Must be assigned only by a Weapon Instance.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageBase;

    /// A constant value of damage that will be added on damage computing. Must be assigned only by a Ranged Weapon
    /// Instance.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageRangedBase;

    /// A constant value of damage that will be added on damage computing. Must be assigned only by an Abilitie.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageAdditive;

    /// A scalar value that will be multiply damage on damage computing. Must be assigned only by an Abilitie.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageMultiplier;
};
