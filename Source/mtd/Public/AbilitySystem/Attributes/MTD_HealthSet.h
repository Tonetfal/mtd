#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "mtd.h"

#include "MTD_HealthSet.generated.h"

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

UCLASS()
class MTD_API UMTD_HealthSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UMTD_HealthSet, Health);
	ATTRIBUTE_ACCESSORS(UMTD_HealthSet, MaxHealth);

protected:
	virtual bool PreGameplayEffectExecute(
		FGameplayEffectModCallbackData &Data) override;
	virtual void PostGameplayEffectExecute(
		const FGameplayEffectModCallbackData &Data) override;

	virtual void PreAttributeBaseChange(
		const FGameplayAttribute &Attribute, float &NewValue) const override;
	virtual void PreAttributeChange(
		const FGameplayAttribute &Attribute, float &NewValue) override;
	virtual void PostAttributeChange(
		const FGameplayAttribute &Attribute, float OldValue, 
		float NewValue) override;

	void ClampAttribute(
    	const FGameplayAttribute &Attribute, float &NewValue) const;
	
public:
	mutable FAttributeEventSignature OnOutOfHealthDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	
	bool bOutOfHealth = false;
};
