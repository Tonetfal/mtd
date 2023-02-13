#include "AbilitySystem/MTD_AbilitySystemComponent.h"

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

void UMTD_AbilitySystemComponent::ProcessAbilityInput(float DeltaSeconds, bool bGamePaused)
{
    // Clear all the inputs if the game has been paused
    if (bGamePaused)
    {
        ClearAbilityInput();
    }

    static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
    AbilitiesToActivate.Reset();

    // Process held input
    for (const FGameplayAbilitySpecHandle &SpecHandle : InputHeldSpecHandles)
    {
        const FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(SpecHandle);
        if (((!Spec) || (!Spec->Ability) || (Spec->IsActive())))
        {
            continue;
        }

        // Add the ability to the activation list if it has to be activated as long as the input is triggered
        const auto Ability = CastChecked<UMTD_GameplayAbility>(Spec->Ability);
        const EMTD_AbilityActivationPolicy ActivationPolicy = Ability->GetActivationPolicy();
        if (ActivationPolicy == EMTD_AbilityActivationPolicy::WhileInputActive)
        {
            AbilitiesToActivate.AddUnique(Spec->Handle);
        }
    }

    // Process triggered input
    for (const FGameplayAbilitySpecHandle &SpecHandle : InputPressedSpecHandles)
    {
        FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(SpecHandle);
        if (((!Spec) || (!IsValid(Spec->Ability))))
        {
            continue;
        }

        // Update the input state for the ability
        Spec->InputPressed = true;

        if (Spec->IsActive())
        {
            // Ability is active, so pass along the input event
            AbilitySpecInputPressed(*Spec);
        }
        else
        {
            // Add the ability to the activation list if it has to be activated when the input has been just pressed
            const auto Ability = CastChecked<UMTD_GameplayAbility>(Spec->Ability);
            const EMTD_AbilityActivationPolicy ActivationPolicy = Ability->GetActivationPolicy();
            if (ActivationPolicy == EMTD_AbilityActivationPolicy::OnInputTriggered)
            {
                AbilitiesToActivate.AddUnique(Spec->Handle);
            }
        }
    }

    // Try to activate all the abilities that have been gather so far
    for (const FGameplayAbilitySpecHandle &SpecHandle : AbilitiesToActivate)
    {
        TryActivateAbility(SpecHandle);
    }

    // Process released input
    for (const FGameplayAbilitySpecHandle &SpecHandle : InputReleasedSpecHandles)
    {
        FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(SpecHandle);
        if (((!Spec) || (!IsValid(Spec->Ability))))
        {
            continue;
        }

        // Update the input state for the ability
        Spec->InputPressed = false;

        if (Spec->IsActive())
        {
            // Ability is active, so pass along the input event
            AbilitySpecInputReleased(*Spec);
        }
    }

    // Clear cached ability handles
    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
}

void UMTD_AbilitySystemComponent::ClearAbilityInput()
{
    InputPressedSpecHandles.Empty();
    InputHeldSpecHandles.Empty();
    InputReleasedSpecHandles.Empty();
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

bool UMTD_AbilitySystemComponent::ChangeGameplayEffectLevelHandle(FActiveGameplayEffectHandle Handle,
    int32 DeltaLevel) const
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

    // Get the active gameplay effect
    FActiveGameplayEffect *ActiveGe = const_cast<FActiveGameplayEffect *>(ConstActiveGe);

    // Add the delta level to the original level, and set the result to the spec
    const float CurrentLevel = ActiveGe->Spec.GetLevel();
    const float NewLevel = (CurrentLevel + DeltaLevel);
    ActiveGe->Spec.SetLevel(NewLevel);

    return true;
}

void UMTD_AbilitySystemComponent::OnAbilityInputTagPressed(const FGameplayTag &InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    // Iterate through all activatable abilities, and check whether it's assosiated with the pressed input tag
    for (const FGameplayAbilitySpec &AbilitySpec : ActivatableAbilities.Items)
    {
        if (((IsValid(AbilitySpec.Ability)) && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))))
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

    // Iterate through all activatable abilities, and check whether it's assosiated with the pressed input tag
    for (const FGameplayAbilitySpec &AbilitySpec : ActivatableAbilities.Items)
    {
        if (((IsValid(AbilitySpec.Ability)) && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))))
        {
            InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
            InputHeldSpecHandles.Remove(AbilitySpec.Handle);
        }
    }
}

void UMTD_AbilitySystemComponent::GiveTagToAbility(const FGameplayTag &InputTag, UMTD_GameplayAbility *Ability)
{
    check(IsValid(Ability));

    // Iterate through all activatable abilities, and check whether it's assosiated with the pressed input tag
    for (FGameplayAbilitySpec &AbilitySpec : ActivatableAbilities.Items)
    {
        const auto OtherAbility = Cast<UMTD_GameplayAbility>(AbilitySpec.Ability);
        
        if (((IsValid(OtherAbility)) && (OtherAbility->GetMainAbilityTag() == Ability->GetMainAbilityTag())))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(InputTag);
            break;
        }
    }
}
