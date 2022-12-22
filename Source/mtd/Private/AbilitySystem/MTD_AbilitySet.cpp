#include "AbilitySystem/MTD_AbilitySet.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect.h"

void FMTD_AbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle &Handle)
{
    if (!Handle.IsValid())
    {
        return;
    }

    AbilitySpecHandles.Add(Handle);
}

void FMTD_AbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle &Handle)
{
    if (!Handle.IsValid())
    {
        return;
    }

    GameplayEffectHandles.Add(Handle);
}

void FMTD_AbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet *Set)
{
    if ((!Set) || (!Set->IsValidLowLevel()))
    {
        return;
    }

    GrantedAttributeSets.Add(Set);
}

void FMTD_AbilitySet_GrantedHandles::TakeFromAbilitySystem(UMTD_AbilitySystemComponent *MtdAsc)
{
    check(MtdAsc);

    for (const FGameplayAbilitySpecHandle &Handle : AbilitySpecHandles)
    {
        if (Handle.IsValid())
        {
            MtdAsc->ClearAbility(Handle);
        }
    }

    for (const FActiveGameplayEffectHandle &Handle : GameplayEffectHandles)
    {
        if (Handle.IsValid())
        {
            MtdAsc->RemoveActiveGameplayEffect(Handle);
        }
    }

    for (UAttributeSet *Set : GrantedAttributeSets)
    {
        MtdAsc->RemoveSpawnedAttribute(Set);
    }

    AbilitySpecHandles.Reset();
    GameplayEffectHandles.Reset();
    GrantedAttributeSets.Reset();
}

void UMTD_AbilitySet::GiveToAbilitySystem(
    UMTD_AbilitySystemComponent *MtdAsc,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
    UObject *SourceObject) const
{
    check(MtdAsc);

    GrantAbilities(MtdAsc, OutGrantedHandles, SourceObject);
    GrantEffects(MtdAsc, OutGrantedHandles, SourceObject);
    GrantAttributes(MtdAsc, OutGrantedHandles, SourceObject);
}

void UMTD_AbilitySet::K2_GiveToAbilitySystem(
    UMTD_AbilitySystemComponent *MtdAsc,
    FMTD_AbilitySet_GrantedHandles &OutGrantedHandles,
    UObject *SourceObject)
{
    GiveToAbilitySystem(MtdAsc, &OutGrantedHandles, SourceObject);
}

void UMTD_AbilitySet::GrantAbilities(
    UMTD_AbilitySystemComponent *MtdAsc,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
    UObject *SourceObject) const
{
    int32 AbilityIndex = 0;

    for (const FMTD_AbilitySet_GameplayAbility &AbilityToGrant : GrantedGameplayAbilities)
    {
        if (!IsValid(AbilityToGrant.Ability))
        {
            MTDS_ERROR("Gameplay ability [%d] is invalid", AbilityIndex);
            AbilityIndex++;
            continue;
        }

        const auto Ability = AbilityToGrant.Ability->GetDefaultObject<UMTD_GameplayAbility>();

        FGameplayAbilitySpec AbilitySpec(Ability, AbilityToGrant.AbilityLevel);
        AbilitySpec.SourceObject = SourceObject;
        AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

        const FGameplayAbilitySpecHandle AbilitySpecHandle = MtdAsc->GiveAbility(AbilitySpec);

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
        }

        AbilityIndex++;
    }
}

void UMTD_AbilitySet::GrantEffects(
    UMTD_AbilitySystemComponent *MtdAsc,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
    UObject *SourceObject) const
{
    int32 EffectIndex = 0;

    for (const FMTD_AbilitySet_GameplayEffect &EffectToGrant : GrantedGameplayEffects)
    {
        if (!IsValid(EffectToGrant.GameplayEffect))
        {
            MTDS_ERROR("Gameplay effect [%d] is invalid", EffectIndex);
            EffectIndex++;
            continue;
        }

        const auto GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UMTD_GameplayEffect>();
        const FActiveGameplayEffectHandle GameplayEffectHandle =
            MtdAsc->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, MtdAsc->MakeEffectContext());

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
        }

        EffectIndex++;
    }
}

void UMTD_AbilitySet::GrantAttributes(
    UMTD_AbilitySystemComponent *MtdAsc,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
    UObject *SourceObject) const
{
    int32 SetIndex = 0;

    for (const FMTD_AbilitySet_AttributeSet &SetToGrant : GrantedAttributes)
    {
        if (!IsValid(SetToGrant.AttributeSet))
        {
            MTDS_ERROR("Gameplay attribute set [%d] is invalid", SetIndex);
            SetIndex++;
            continue;
        }

        const auto NewSet = NewObject<UMTD_AttributeSet>(MtdAsc->GetOwner(), SetToGrant.AttributeSet);
        MtdAsc->AddAttributeSetSubobject(NewSet);

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddAttributeSet(NewSet);
        }

        SetIndex++;
    }
}
