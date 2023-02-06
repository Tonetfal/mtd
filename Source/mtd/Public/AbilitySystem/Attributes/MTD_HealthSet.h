#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_HealthSet.generated.h"

/**
 * Attribute set that defines all the life related required data for a Pawn that makes use of the Ability System
 * Component.
 */
UCLASS()
class MTD_API UMTD_HealthSet : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UMTD_HealthSet, Health);
    ATTRIBUTE_ACCESSORS(UMTD_HealthSet, MaxHealth);
    ATTRIBUTE_ACCESSORS(UMTD_HealthSet, LastLostHealth_Meta);

protected:
    virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) override;

    virtual void PreAttributeBaseChange(const FGameplayAttribute &Attribute, float &NewValue) const override;
    virtual void PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) override;
    virtual void PostAttributeBaseChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) const override;

    void ClampAttribute(const FGameplayAttribute &Attribute, float &NewValue) const;

public:
    mutable FAttributeEventSignature OnOutOfHealthDelegate;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Health;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData MaxHealth;

    /** Meta attribute. The most recent amount of health lost (may be updated several times per frame). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData LastLostHealth_Meta;

    bool bOutOfHealth = false;
};
