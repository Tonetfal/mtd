#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "mtd.h"

#include "MTD_AttributeSet.generated.h"

class AActor;
struct FGameplayEffectSpec;

DECLARE_MULTICAST_DELEGATE_FourParams(
    FAttributeEventSignature,
    AActor* /*EffectInstigator*/,
    AActor* /*EffectCauser*/,
    const FGameplayEffectSpec& /*EffectSpec*/,
    float /*EffectMagnitude*/);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define CAPTURE_ATTRIBUTE(ATTRIBUTE_SET, ATTRIBUTE, CAPTURE_SOURCE, SNAPSHOT) \
    FGameplayEffectAttributeCaptureDefinition( \
    ATTRIBUTE_SET ## ::Get ## ATTRIBUTE ## Attribute(), \
    EGameplayEffectAttributeCaptureSource:: ## CAPTURE_SOURCE, \
    SNAPSHOT);

/**
 * Default attribute set used in this project.
 */
UCLASS(meta=(ToolTip="Default attribute set used in this project."))
class MTD_API UMTD_AttributeSet : public UAttributeSet
{
    GENERATED_BODY()
};

AActor *GetInstigatorFromAttrChangeData(const FOnAttributeChangeData &ChangeData);
