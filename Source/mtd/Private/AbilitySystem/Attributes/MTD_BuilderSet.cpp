#include "AbilitySystem/Attributes/MTD_BuilderSet.h"

#define MAP(x) BonusAttributeReplicationMapping.Add(Get ## x ## Stat_BonusAttribute(), Get ## x ## StatAttribute())
UMTD_BuilderSet::UMTD_BuilderSet()
{
    MAP(Damage);
    MAP(Health);
    MAP(Range);
    MAP(Speed);
}
#undef MAP

void UMTD_BuilderSet::PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);
    
    if (const auto &MappedValue = BonusAttributeReplicationMapping.Find(Attribute))
    {
        FGameplayAttributeData *AttributeData = MappedValue->GetGameplayAttributeData(this);
        check(AttributeData);
        
        const float Delta = (NewValue - OldValue);
        const float CurrentValue = AttributeData->GetCurrentValue();
        const float FinalValue = (CurrentValue + Delta);
        AttributeData->SetCurrentValue(FinalValue);
    }
}
