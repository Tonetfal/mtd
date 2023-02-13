#include "Character/MTD_ManaComponent.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"

UMTD_ManaComponent::UMTD_ManaComponent()
{
    // There is nothing to tick for
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_ManaComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent)
{
    Super::InitializeWithAbilitySystem(InAbilitySystemComponent);
    if (!AbilitySystemComponent)
    {
        return;
    }

    // Cache the mana set to avoid searching for it in ability system component every time it's needed
    ManaSet = AbilitySystemComponent->GetSet<UMTD_ManaSet>();
    if (!ManaSet)
    {
        MTDS_ERROR("Cannot initialize mana component with NULL mana set on the ability system.");
        return;
    }

    // Listen for mana and max mana attribute changes
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_ManaSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_ManaSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::OnMaxManaChanged);

    // Notify about initial values
    OnManaChangedDelegate.Broadcast(this, ManaSet->GetMana(), ManaSet->GetMana(), nullptr);
    OnMaxManaChangedDelegate.Broadcast(this, ManaSet->GetMaxMana(), ManaSet->GetMaxMana(), nullptr);
}

void UMTD_ManaComponent::UninitializeFromAbilitySystem()
{
    // Nullify ability system related data
    ManaSet = nullptr;

    Super::UninitializeFromAbilitySystem();
}

float UMTD_ManaComponent::GetMana() const
{
    return ((IsValid(ManaSet)) ? (ManaSet->GetMana()) : (0.f));
}

float UMTD_ManaComponent::GetMaxMana() const
{
    return ((IsValid(ManaSet)) ? (ManaSet->GetMaxMana()) : (0.f));
}

float UMTD_ManaComponent::GetManaNormilized() const
{
    if (IsValid(ManaSet))
    {
        const float Mana = ManaSet->GetMana();
        const float MaxMana = ManaSet->GetMaxMana();

        return ((MaxMana > 0.f) ? (Mana / MaxMana) : (0.f));
    }
    return 0.f;
}

bool UMTD_ManaComponent::IsManaFull() const
{
    return ((IsValid(ManaSet)) ? (ManaSet->GetMana() == ManaSet->GetMaxMana()) : (false));
}

void UMTD_ManaComponent::OnManaChanged(const FOnAttributeChangeData &ChangeData)
{
    OnManaChangedDelegate.Broadcast(
        this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_ManaComponent::OnMaxManaChanged(const FOnAttributeChangeData &ChangeData)
{
    OnMaxManaChangedDelegate.Broadcast(
        this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}
