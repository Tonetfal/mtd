#include "Character/MTD_AscComponent.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"

UMTD_AscComponent::UMTD_AscComponent()
{
}

void UMTD_AscComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent)
{
    const AActor *Owner = GetOwner();
    check(IsValid(Owner));

    // Make sure that we're not initialize with an ability system component yet
    if (IsValid(AbilitySystemComponent))
    {
        MTDS_ERROR("ASC component for owner [%s] has already been initilized with an ability system.",
            *Owner->GetName());
        return;
    }

    // Use the given ability system component
    AbilitySystemComponent = InAbilitySystemComponent;
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_ERROR("Cannot initilize balance component for owner [%s] with an invalid ability system.",
            *Owner->GetName());
        return;
    }
}

void UMTD_AscComponent::UninitializeFromAbilitySystem()
{
    // Nullify ability system
    AbilitySystemComponent = nullptr;
}

void UMTD_AscComponent::OnUnregister()
{
    UninitializeFromAbilitySystem();
    Super::OnUnregister();
}
