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
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, LastReceivedDamage);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, Threshold);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, BaseDamage);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, Resist);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, KnockbackDirectionX);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, KnockbackDirectionY);
    ATTRIBUTE_ACCESSORS(UMTD_BalanceSet, KnockbackDirectionZ);

protected:
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) override;

public:
    mutable FAttributeEventSignature OnBalanceDownDelegate;

protected:
    /** Amount of last received balance damage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData LastReceivedDamage;

    /** Resistable amount of damage balance. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Threshold;

    /** Amount of damage dealt with a hit. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData BaseDamage;

    /** Percentage of how much damage is ignored. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Resist;

    /** X component of knockback direction normal vector. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData KnockbackDirectionX;

    /** Y component of knockback direction normal vector. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData KnockbackDirectionY;

    /** Z component of knockback direction normal vector. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData KnockbackDirectionZ;
};
