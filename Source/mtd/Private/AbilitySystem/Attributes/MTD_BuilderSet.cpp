#include "AbilitySystem/Attributes/MTD_BuilderSet.h"

UMTD_BuilderSet::UMTD_BuilderSet()
{
    // Bind corresponding attribute (Health -> BonusHealth, Damage -> BonusDamage, ...)
#define MAP(x) BonusAttributeReplicationMapping.Add(Get ## x ## Stat_BonusAttribute(), Get ## x ## StatAttribute())
    MAP(Damage);
    MAP(Health);
    MAP(Range);
    MAP(Speed);
#undef MAP
}

void UMTD_BuilderSet::PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    // Check if changed attribute is a bonus one
    const auto &MappedValue = BonusAttributeReplicationMapping.Find(Attribute);
    if (MappedValue)
    {
        // Get the base attribute data, i.e. if HealthBonus has been changed, this will get Health attribute
        FGameplayAttributeData *AttributeData = MappedValue->GetGameplayAttributeData(this);
        check(AttributeData);

        // Modify the base attribute by the bonus attribute delta
        const float Delta = (NewValue - OldValue);
        const float CurrentValue = AttributeData->GetCurrentValue();
        const float FinalValue = (CurrentValue + Delta);
        AttributeData->SetCurrentValue(FinalValue);
    }
}
