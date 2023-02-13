#include "AbilitySystem/Attributes/MTD_HealthSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

void UMTD_HealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        const float HealthValue = GetHealth();
        
        // Check if owner should die
        if (((HealthValue <= 0.f) && (!bOutOfHealth)))
        {
            if (OnOutOfHealthDelegate.IsBound())
            {
                // Prepare data to send about death
                const FGameplayEffectContextHandle &EffectContext = Data.EffectSpec.GetEffectContext();
                AActor *Instigator = EffectContext.GetOriginalInstigator();
                AActor *Causer = EffectContext.GetEffectCauser();

                // Notify about death
                OnOutOfHealthDelegate.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
            }
        }

        // Update death state
        bOutOfHealth = (HealthValue <= 0.f);
    }
}

void UMTD_HealthSet::PreAttributeBaseChange(const FGameplayAttribute &Attribute, float &NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

    ClampAttribute(Attribute, NewValue);
}

void UMTD_HealthSet::PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    ClampAttribute(Attribute, NewValue);
}

void UMTD_HealthSet::PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetMaxHealthAttribute())
    {
        // Clamp current health with new max health in mind
        const float HealthValue = GetHealth();
        if (HealthValue > NewValue)
        {
            UAbilitySystemComponent *AbilitySystem = GetOwningAbilitySystemComponentChecked();

            AbilitySystem->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
        }
    }

    if (((bOutOfHealth) && (GetHealth() > 0.f)))
    {
        bOutOfHealth = false;
    }
}

void UMTD_HealthSet::PostAttributeBaseChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) const
{
    Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
    
    if (Attribute == GetMaxHealthAttribute())
    {
        // Clamp current health with new max health in mind
        const float HealthValue = GetHealth();
        if (HealthValue > NewValue)
        {
            UAbilitySystemComponent *AbilitySystem = GetOwningAbilitySystemComponentChecked();

            AbilitySystem->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
        }
    }
}

void UMTD_HealthSet::ClampAttribute(const FGameplayAttribute &Attribute, float &NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        // Range [0, MaxHealth]
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    else if (Attribute == GetMaxHealthAttribute())
    {
        // Range [1, MAX_FLOAT]
        NewValue = FMath::Max(NewValue, 1.f);
    }
}
