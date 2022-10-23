#pragma once

#include "mtd.h"
#include "AbilitySystem/Attributes/MTD_AttributeSet.h"

#include "MTD_HealthSet.generated.h"

/**
 * Attribute set that defines all the life related required data for a Pawn that
 * makes use of the Ability System Component.
 */
UCLASS()
class MTD_API UMTD_HealthSet : public UMTD_AttributeSet
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
