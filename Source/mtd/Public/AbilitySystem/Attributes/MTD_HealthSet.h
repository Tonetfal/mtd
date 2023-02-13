#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_HealthSet.generated.h"

/**
 * Attribute set containing all the owner's health related data.
 */
UCLASS()
class MTD_API UMTD_HealthSet
    : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UMTD_HealthSet, Health);
    ATTRIBUTE_ACCESSORS(UMTD_HealthSet, MaxHealth);
    ATTRIBUTE_ACCESSORS(UMTD_HealthSet, LastLostHealth_Meta);

protected:
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) override;

    virtual void PreAttributeBaseChange(const FGameplayAttribute &Attribute, float &NewValue) const override;
    virtual void PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) override;
    virtual void PostAttributeBaseChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) const override;

    void ClampAttribute(const FGameplayAttribute &Attribute, float &NewValue) const;

public:
    /**
     * Delegate to fire when owner's health 0.
     * 
     * The delegate must be mutable because it's used from outside, and non-ASC code only can have const pointers to
     * an attribute set.
     */
    mutable FAttributeEventSignature OnOutOfHealthDelegate;

protected:
    /** Current health value. Is always in range of [0, MaxHealth]. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Health;

    /** Max health value. Defines the highest bound for health value. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData MaxHealth;

    /** Meta attribute. The most recent amount of health lost (may be updated several times per frame). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData LastLostHealth_Meta;

    /** If true, owner should die, false otherwise. Is used to avoid triggering death multiple times. */
    bool bOutOfHealth = false;
};
