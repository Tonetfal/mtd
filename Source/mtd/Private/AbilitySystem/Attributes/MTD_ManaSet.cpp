#include "AbilitySystem/Attributes/MTD_ManaSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

void UMTD_ManaSet::PreAttributeBaseChange(
	const FGameplayAttribute &Attribute, float &NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UMTD_ManaSet::PreAttributeChange(
	const FGameplayAttribute &Attribute, float &NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}	

void UMTD_ManaSet::PostAttributeChange(
	const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxManaAttribute())
	{
		// Decrease current mana if max mana has decreased
		if (GetMana() > NewValue)
		{
			UAbilitySystemComponent *AbilitySystem = 
				GetOwningAbilitySystemComponentChecked();

			AbilitySystem->ApplyModToAttribute(
				GetManaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void UMTD_ManaSet::ClampAttribute(
	const FGameplayAttribute &Attribute, float &NewValue) const
{
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
