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

void FMTD_AbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet *AttributeSet)
{
    if (((!IsValid(AttributeSet)) || (!AttributeSet->IsValidLowLevel())))
    {
        return;
    }

    GrantedAttributeSets.Add(AttributeSet);
}

void FMTD_AbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle &Handle)
{
    if (!Handle.IsValid())
    {
        return;
    }

    GameplayEffectHandles.Add(Handle);
}

void FMTD_AbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent *AbilitySystemComponent)
{
    check(IsValid(AbilitySystemComponent));

    // Take back all the gameplay effects
    for (const FActiveGameplayEffectHandle &Handle : GameplayEffectHandles)
    {
        if (Handle.IsValid())
        {
            AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
        }
    }

    // Take back all the attribute sets
    for (UAttributeSet *Set : GrantedAttributeSets)
    {
        AbilitySystemComponent->RemoveSpawnedAttribute(Set);
    }

    // Take back all the gameplay abilities
    for (const FGameplayAbilitySpecHandle &Handle : AbilitySpecHandles)
    {
        if (Handle.IsValid())
        {
            AbilitySystemComponent->ClearAbility(Handle);
        }
    }

    // Reset all the containing data
    AbilitySpecHandles.Reset();
    GameplayEffectHandles.Reset();
    GrantedAttributeSets.Reset();
}

void UMTD_AbilitySet::GiveToAbilitySystem(UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles, UObject *SourceObject) const
{
    check(IsValid(MtdAbilitySystemComponent));

    GrantAbilities(MtdAbilitySystemComponent, OutGrantedHandles, SourceObject);
    GrantAttributes(MtdAbilitySystemComponent, OutGrantedHandles, SourceObject);

    // Effects should be given after attribute sets because the effects can be related to them
    GrantEffects(MtdAbilitySystemComponent, OutGrantedHandles, SourceObject);
}

void UMTD_AbilitySet::K2_GiveToAbilitySystem(UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
    FMTD_AbilitySet_GrantedHandles &OutGrantedHandles, UObject *SourceObject)
{
    GiveToAbilitySystem(MtdAbilitySystemComponent, &OutGrantedHandles, SourceObject);
}

void UMTD_AbilitySet::GrantAbilities(UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles, UObject *SourceObject) const
{
    int32 AbilityIndex = 0;
    
    // Iterate through all abilities, and grant them
    for (const FMTD_AbilitySet_GameplayAbility &AbilityToGrant : GrantedGameplayAbilities)
    {
        if (!IsValid(AbilityToGrant.Ability))
        {
            MTDS_ERROR("Gameplay ability [%d] is invalid", AbilityIndex);
            AbilityIndex++;
            continue;
        }

        // Create the ability
        const auto Ability = AbilityToGrant.Ability->GetDefaultObject<UMTD_GameplayAbility>();

        // Create the ability spec to activate the ability from
        FGameplayAbilitySpec AbilitySpec(Ability, AbilityToGrant.AbilityLevel);
        AbilitySpec.SourceObject = SourceObject;

        // Assosiate an input tag with this ability
        AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

        // Give the ability spec to the ability system component
        const FGameplayAbilitySpecHandle AbilitySpecHandle = MtdAbilitySystemComponent->GiveAbility(AbilitySpec);

        if (OutGrantedHandles)
        {
            // Save the spec handle
            OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
        }

        AbilityIndex++;
    }
}

void UMTD_AbilitySet::GrantAttributes(UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles, UObject *SourceObject) const
{
    int32 SetIndex = 0;

    // Iterate through all attribute sets, and grant them
    for (const FMTD_AbilitySet_AttributeSet &SetToGrant : GrantedAttributes)
    {
        if (!IsValid(SetToGrant.AttributeSet))
        {
            MTDS_ERROR("Gameplay attribute set [%d] is invalid", SetIndex);
            SetIndex++;
            continue;
        }

        // Create the set
        const auto NewSet = NewObject<UMTD_AttributeSet>(MtdAbilitySystemComponent->GetOwner(), SetToGrant.AttributeSet);

        // Give the attribute set to the ability system component
        MtdAbilitySystemComponent->AddAttributeSetSubobject(NewSet);

        if (OutGrantedHandles)
        {
            // Save the attribute set pointer
            OutGrantedHandles->AddAttributeSet(NewSet);
        }

        SetIndex++;
    }
}

void UMTD_AbilitySet::GrantEffects(UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
    FMTD_AbilitySet_GrantedHandles *OutGrantedHandles, UObject *SourceObject) const
{
    int32 EffectIndex = 0;

    // Iterate through all gameplay effects, and grant them
    for (const FMTD_AbilitySet_GameplayEffect &EffectToGrant : GrantedGameplayEffects)
    {
        if (!IsValid(EffectToGrant.GameplayEffect))
        {
            MTDS_ERROR("Gameplay effect [%d] is invalid", EffectIndex);
            EffectIndex++;
            continue;
        }

        // Create the effect
        const auto GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UMTD_GameplayEffect>();

        // Give the effect to the ability system component
        const FActiveGameplayEffectHandle GameplayEffectHandle = MtdAbilitySystemComponent->ApplyGameplayEffectToSelf(
            GameplayEffect, EffectToGrant.EffectLevel, MtdAbilitySystemComponent->MakeEffectContext());

        if (OutGrantedHandles)
        {
            // Save the effect handle
            OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
        }

        EffectIndex++;
    }
}
