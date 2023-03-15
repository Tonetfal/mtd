#include "Character/Components/MTD_HealthComponent.h"

#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "GameplayEffectExtension.h"

UMTD_HealthComponent::UMTD_HealthComponent()
{
    // Nothing to tick for
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_HealthComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent)
{
    Super::InitializeWithAbilitySystem(InAbilitySystemComponent);
    if (!AbilitySystemComponent)
    {
        return;
    }

    // Cache the health set to avoid searching for it in ability system component every time it's needed
    HealthSet = AbilitySystemComponent->GetSet<UMTD_HealthSet>();
    if (!HealthSet)
    {
        MTDS_ERROR("Cannot initialize health component with NULL health set on the ability system");
        return;
    }

    // Listen for health and max health attribute changes
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_HealthSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_HealthSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

    // Listen for out of health event on health set
    HealthSet->OnOutOfHealthDelegate.AddUObject(this, &ThisClass::OnOutOfHealth);

    // Notify about initial values
    OnHealthChangedDelegate.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
    OnMaxHealthChangedDelegate.Broadcast(this, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth(), nullptr);
}

void UMTD_HealthComponent::UninitializeFromAbilitySystem()
{
    if (IsValid(HealthSet))
    {
        HealthSet->OnOutOfHealthDelegate.RemoveAll(this);
    }

    // Nullify ability system related data
    HealthSet = nullptr;

    Super::UninitializeFromAbilitySystem();
}

float UMTD_HealthComponent::GetHealth() const
{
    return ((IsValid(HealthSet)) ? (HealthSet->GetHealth()) : (0.f));
}

float UMTD_HealthComponent::GetMaxHealth() const
{
    return ((IsValid(HealthSet)) ? (HealthSet->GetMaxHealth()) : (0.f));
}

float UMTD_HealthComponent::GetHealthNormilized() const
{
    if (IsValid(HealthSet))
    {
        const float Health = HealthSet->GetHealth();
        const float MaxHealth = HealthSet->GetMaxHealth();

        return ((MaxHealth > 0.f) ? (Health / MaxHealth) : (0.f));
    }
    return 0.f;
}

void UMTD_HealthComponent::StartDeath()
{
    // Avoid starting death process if we are already dead
    if (DeathState != EMTD_DeathState::NotDead)
    {
        return;
    }

    // Start dying
    DeathState = EMTD_DeathState::DeathStarted;

    AActor *Owner = GetOwner();
    check(IsValid(Owner));

    // Notify about death start
    OnDeathStarted.Broadcast(Owner);
}

void UMTD_HealthComponent::FinishDeath()
{
    // Avoid finishing death process if it isn't in process
    if (DeathState != EMTD_DeathState::DeathStarted)
    {
        return;
    }

    // Finish dying
    DeathState = EMTD_DeathState::DeathFinished;

    AActor *Owner = GetOwner();
    check(IsValid(Owner));

    // Notify about death finish
    OnDeathFinished.Broadcast(Owner);
}

void UMTD_HealthComponent::SelfDestruct(bool bFeelOutOfWorld)
{
    if (DeathState == EMTD_DeathState::NotDead)
    {
        MTDS_LOG("Already dead.");
        return;
    }

    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability system component is invalid.");
        return;
    }

    // Multiply own health by 0, so that it will be 0
    AbilitySystemComponent->ApplyModToAttribute(
        UMTD_HealthSet::GetHealthAttribute(), EGameplayModOp::Multiplicitive, 0.f);
}

void UMTD_HealthComponent::OnHealthChanged(const FOnAttributeChangeData &ChangeData)
{
    OnHealthChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue,
        GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_HealthComponent::OnMaxHealthChanged(const FOnAttributeChangeData &ChangeData)
{
    OnMaxHealthChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue,
        GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_HealthComponent::OnOutOfHealth(AActor *DamageInstigator, AActor *DamageCauser,
    const FGameplayEffectSpec &DamageEffectSpec, float DamageMagnitude)
{
    if (!IsValid(AbilitySystemComponent))
    {
        return;
    }

    // Send the "Gameplay.Event.Death" gameplay event through the owner's ability system.
    // This can be used to trigger a death gameplay ability.
    FGameplayEventData Payload;
    Payload.EventTag = FMTD_GameplayTags::Get().Gameplay_Event_Death;
    Payload.Instigator = DamageInstigator;
    Payload.Target = AbilitySystemComponent->GetAvatarActor();
    Payload.OptionalObject = DamageEffectSpec.Def;
    Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
    Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
    Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
    Payload.EventMagnitude = DamageMagnitude;

    // Send a gameplay event containing different death related data
    AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}
