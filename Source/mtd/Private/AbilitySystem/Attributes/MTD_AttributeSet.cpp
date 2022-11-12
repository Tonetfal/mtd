#include "AbilitySystem/Attributes/MTD_AttributeSet.h"

#include "GameplayEffectExtension.h"

AActor *GetInstigatorFromAttrChangeData(const FOnAttributeChangeData &ChangeData)
{
    if (ChangeData.GEModData != nullptr)
    {
        const FGameplayEffectContextHandle &EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
        return EffectContext.GetOriginalInstigator();
    }

    return nullptr;
}
