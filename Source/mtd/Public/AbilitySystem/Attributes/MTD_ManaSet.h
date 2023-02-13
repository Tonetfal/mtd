#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_ManaSet.generated.h"

/**
 * Attribute set containing all the owner's mana related data.
 */
UCLASS()
class MTD_API UMTD_ManaSet
    : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UMTD_ManaSet, Mana);
    ATTRIBUTE_ACCESSORS(UMTD_ManaSet, MaxMana);

protected:
    virtual void PreAttributeBaseChange(const FGameplayAttribute &Attribute, float &NewValue) const override;
    virtual void PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) override;

    void ClampAttribute(const FGameplayAttribute &Attribute, float &NewValue) const;

protected:
    /** Current mana value. Is always in range of [0, MaxMana]. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Mana;

    /** Max mana value. Defines the highest bound for mana value. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData MaxMana;
};
