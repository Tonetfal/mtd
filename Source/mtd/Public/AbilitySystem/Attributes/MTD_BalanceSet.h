#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_BalanceSet.generated.h"

/**
 * Attribute set that defines all the balance related required data for a Character.
 */
UCLASS()
class MTD_API UMTD_BalanceSet : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, Damage);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, Threshold);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, Resist);
    
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, LastReceivedBalanceDamage_Meta);
    
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, KnockbackDirectionX_Meta);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, KnockbackDirectionY_Meta);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, KnockbackDirectionZ_Meta);

protected:
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) override;

public:
    mutable FAttributeEventSignature OnBalanceDownDelegate;

protected:
    /** Amount of damage dealt with a hit. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Damage;
    
    /** Resistable amount of damage balance. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Threshold;

    /** Percentage of how much damage is ignored. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Resist;

    /** Meta attribute. Amount of last received balance damage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData LastReceivedBalanceDamage_Meta;

    /** Meta attribute. X component of knockback direction normal vector. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData KnockbackDirectionX_Meta;

    /** Meta attribute. Y component of knockback direction normal vector. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData KnockbackDirectionY_Meta;

    /** Meta attribute. Z component of knockback direction normal vector. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData KnockbackDirectionZ_Meta;
};
