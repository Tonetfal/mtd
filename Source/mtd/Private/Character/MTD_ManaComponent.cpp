#include "Character/MTD_ManaComponent.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"

UMTD_ManaComponent::UMTD_ManaComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_ManaComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc)
{
    const AActor *Owner = GetOwner();
    check(Owner);

    if (AbilitySystemComponent)
    {
        MTDS_ERROR("Mana component for owner [%s] has already been initilized with an ability system",
            *Owner->GetName());
        return;
    }

    AbilitySystemComponent = Asc;
    if (!AbilitySystemComponent)
    {
        MTDS_ERROR("Cannot initilize health component for owner [%s] with a NULL ability system", *Owner->GetName());
        return;
    }

    ManaSet = AbilitySystemComponent->GetSet<UMTD_ManaSet>();
    if (!ManaSet)
    {
        MTDS_ERROR("Cannot initialize health component with NULL combat set on the ability system");
        return;
    }

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_ManaSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_ManaSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::OnMaxManaChanged);

    OnManaChangedDelegate.Broadcast(this, ManaSet->GetMana(), ManaSet->GetMana(), nullptr);
    OnMaxManaChangedDelegate.Broadcast(this, ManaSet->GetMaxMana(), ManaSet->GetMaxMana(), nullptr);
}

void UMTD_ManaComponent::UninitializeFromAbilitySystem()
{
    ManaSet = nullptr;
    AbilitySystemComponent = nullptr;
}

float UMTD_ManaComponent::GetMana() const
{
    return (IsValid(ManaSet)) ? (ManaSet->GetMana()) : (0.f);
}

float UMTD_ManaComponent::GetMaxMana() const
{
    return (IsValid(ManaSet)) ? (ManaSet->GetMaxMana()) : (0.f);
}

float UMTD_ManaComponent::GetManaNormilized() const
{
    if (IsValid(ManaSet))
    {
        const float Mana = ManaSet->GetMana();
        const float MaxMana = ManaSet->GetMaxMana();

        return (MaxMana > 0.f) ? (Mana / MaxMana) : (0.f);
    }
    return 0.f;
}

void UMTD_ManaComponent::OnUnregister()
{
    UninitializeFromAbilitySystem();

    Super::OnUnregister();
}

void UMTD_ManaComponent::OnManaChanged(
    const FOnAttributeChangeData &ChangeData)
{
    OnManaChangedDelegate.Broadcast(
        this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_ManaComponent::OnMaxManaChanged(
    const FOnAttributeChangeData &ChangeData)
{
    OnMaxManaChangedDelegate.Broadcast(
        this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}
