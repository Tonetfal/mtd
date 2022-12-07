#include "Equipment/MTD_WeaponInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"

void UMTD_WeaponInstance::ModStats(float Multiplier)
{
    Super::ModStats(Multiplier);

    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    if (!IsValid(Asc))
    {
        return;
    }

    check(Asc->GetAttributeSet(UMTD_CombatSet::StaticClass()));

    Asc->ApplyModToAttribute(UMTD_CombatSet::GetDamageBaseAttribute(),
        EGameplayModOp::Additive, WeaponStats.BaseDamage * Multiplier);
}

TArray<FGameplayEffectSpecHandle> UMTD_WeaponInstance::GetGameplayEffectSpecHandlesToGrantOnHit() const
{
    TArray<FGameplayEffectSpecHandle> Result;

    AActor *GeOwner = GetOwner();
    if (!IsValid(GeOwner))
    {
        return Result;
    }

    const UAbilitySystemComponent *Asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GeOwner);
    if (!IsValid(Asc))
    {
        return Result;
    }

    const FGameplayEffectContextHandle GeContextHandle = Asc->MakeEffectContext();

    for (const TSubclassOf<UGameplayEffect> GeClass : GameplayEffectsToGrantOnHit)
    {
        if (!GeClass)
        {
            continue;
        }

        Result.Add(Asc->MakeOutgoingSpec(GeClass, 1.f, GeContextHandle));
    }

    return Result;
}
