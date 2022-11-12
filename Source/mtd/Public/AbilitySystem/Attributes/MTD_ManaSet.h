#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_ManaSet.generated.h"

/**
 * Attribute set that defines all the mana related required data for a Player.
 */
UCLASS()
class MTD_API UMTD_ManaSet : public UMTD_AttributeSet
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Mana;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData MaxMana;
};
