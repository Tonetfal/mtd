#include "AbilitySystem/Attributes/MTD_BalanceSet.h"

#include "GameplayEffectExtension.h"

void UMTD_BalanceSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    if (Data.EvaluatedData.Attribute == GetLastReceivedDamageAttribute())
    {
        const float ThresholdValue = Threshold.GetCurrentValue();

        // Notify about balance down if:
        // - we're willing to be knockback-able (Threshold >= 0)
        // - last damage is enough to surpass the threshold
        if ((ThresholdValue >= 0.f) && (ThresholdValue <= Data.EvaluatedData.Magnitude))
        {
            if (OnBalanceDownDelegate.IsBound())
            {
                const FGameplayEffectContextHandle &EffectContext = Data.EffectSpec.GetEffectContext();
                AActor *Instigator = EffectContext.GetOriginalInstigator();
                AActor *Causer = EffectContext.GetEffectCauser();

                const float ResistValue = Resist.GetCurrentValue();
                const float DamageValue = Data.EvaluatedData.Magnitude;
                const float IgnoredDamage = DamageValue / 100.f * ResistValue;
                
                OnBalanceDownDelegate.Broadcast(Instigator, Causer, Data.EffectSpec, DamageValue - IgnoredDamage);
            }
        }
    }
}
