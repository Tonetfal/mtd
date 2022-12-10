#include "AbilitySystem/MTD_AbilitySystemComponent.h"

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

void UMTD_AbilitySystemComponent::ProcessAbilityInput(float DeltaSeconds, bool bGamePaused)
{
    // TODO: Clear ability input if paused
    // ...

    static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
    AbilitiesToActivate.Reset();

    // Process held input
    for (const FGameplayAbilitySpecHandle &SpecHandle : InputHeldSpecHandles)
    {
        const FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(SpecHandle);
        if ((!Spec) || (!Spec->Ability) || (Spec->IsActive()))
        {
            continue;
        }

        const auto Ability = CastChecked<UMTD_GameplayAbility>(Spec->Ability);

        if (Ability->GetActivationPolicy() == EMTD_AbilityActivationPolicy::WhileInputActive)
        {
            AbilitiesToActivate.AddUnique(Spec->Handle);
        }
    }

    // Process triggered input
    for (const FGameplayAbilitySpecHandle &SpecHandle : InputPressedSpecHandles)
    {
        FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(SpecHandle);
        if ((!Spec) || (!Spec->Ability))
        {
            continue;
        }

        Spec->InputPressed = true;
        if (Spec->IsActive())
        {
            // Ability is active so pass along the input event
            AbilitySpecInputPressed(*Spec);
        }
        else
        {
            const auto Ability = CastChecked<UMTD_GameplayAbility>(Spec->Ability);
            check(Ability);

            if (Ability->GetActivationPolicy() == EMTD_AbilityActivationPolicy::OnInputTriggered)
            {
                AbilitiesToActivate.AddUnique(Spec->Handle);
            }
        }

        const auto Ability = CastChecked<UMTD_GameplayAbility>(Spec->Ability);
        check(Ability);
    }

    for (const FGameplayAbilitySpecHandle &SpecHandle : AbilitiesToActivate)
    {
        TryActivateAbility(SpecHandle);
    }

    for (const FGameplayAbilitySpecHandle &SpecHandle : InputReleasedSpecHandles)
    {
        FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(SpecHandle);
        if ((!Spec) || (!Spec->Ability))
        {
            continue;
        }

        Spec->InputPressed = false;

        if (Spec->IsActive())
        {
            // Ability is active so pass along the input event
            AbilitySpecInputReleased(*Spec);
        }
    }

    // Clear cached ability handles
    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
}

bool UMTD_AbilitySystemComponent::SetGameplayEffectDurationHandle(FActiveGameplayEffectHandle Handle, float NewDuration)
{
    if (!Handle.IsValid())
    {
        return false;
    }

    const FActiveGameplayEffect *ConstActiveGe = GetActiveGameplayEffect(Handle);
    if (!ConstActiveGe)
    {
        return false;
    }

    FActiveGameplayEffect *ActiveGe = const_cast<FActiveGameplayEffect *>(ConstActiveGe);
    if (NewDuration > 0.f)
    {
        ActiveGe->Spec.Duration = NewDuration;
    }
    else
    {
        ActiveGe->Spec.Duration = 0.01f;
    }

    ActiveGe->StartServerWorldTime = ActiveGameplayEffects.GetServerWorldTime();
    ActiveGe->CachedStartServerWorldTime = ActiveGe->StartServerWorldTime;
    ActiveGe->StartWorldTime = ActiveGameplayEffects.GetWorldTime();
    ActiveGameplayEffects.MarkItemDirty(*ActiveGe);
    ActiveGameplayEffects.CheckDuration(Handle);

    ActiveGe->EventSet.OnTimeChanged.Broadcast(ActiveGe->Handle, ActiveGe->StartWorldTime, ActiveGe->GetDuration());
    OnGameplayEffectDurationChange(*ActiveGe);

    return true;
}

bool UMTD_AbilitySystemComponent::IncreaseGameplayEffectLevelHandle(
    FActiveGameplayEffectHandle Handle, float IncreaseBy) const
{
    if (!Handle.IsValid())
    {
        return false;
    }

    const FActiveGameplayEffect *ConstActiveGe = GetActiveGameplayEffect(Handle);
    if (!ConstActiveGe)
    {
        return false;
    }

    FActiveGameplayEffect *ActiveGe = const_cast<FActiveGameplayEffect *>(ConstActiveGe);

    const float OldLevel = ActiveGe->Spec.GetLevel();
    const float NewLevel = OldLevel + IncreaseBy;

    ActiveGe->Spec.SetLevel(NewLevel);

    return true;
}

void UMTD_AbilitySystemComponent::OnAbilityInputTagPressed(const FGameplayTag &InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    for (const FGameplayAbilitySpec &AbilitySpec : ActivatableAbilities.Items)
    {
        if ((IsValid(AbilitySpec.Ability)) && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
        {
            InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
            InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
        }
    }
}

void UMTD_AbilitySystemComponent::OnAbilityInputTagReleased(const FGameplayTag &InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    for (const FGameplayAbilitySpec &AbilitySpec : ActivatableAbilities.Items)
    {
        if ((IsValid(AbilitySpec.Ability)) && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
        {
            InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
            InputHeldSpecHandles.Remove(AbilitySpec.Handle);
        }
    }
}
