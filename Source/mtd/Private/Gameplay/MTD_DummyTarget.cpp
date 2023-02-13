#include "Gameplay/MTD_DummyTarget.h"

#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "MTD_CoreTypes.h"
#include "System/MTD_Tags.h"

AMTD_DummyTarget::AMTD_DummyTarget()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    AbilitySystemComponent = CreateDefaultSubobject<UMTD_AbilitySystemComponent>("MTD Ability System Component");

    // Target has to have health to receive damage
    UMTD_HealthSet *HealthSet = CreateDefaultSubobject<UMTD_HealthSet>("Health Set");
    AbilitySystemComponent->AddSpawnedAttribute(HealthSet);

    Tags.Add(FMTD_Tags::Foe);
}

void AMTD_DummyTarget::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Listen for LostHealth meta attribute changes
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_HealthSet::GetLastLostHealth_MetaAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

UAbilitySystemComponent *AMTD_DummyTarget::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

FGenericTeamId AMTD_DummyTarget::GetGenericTeamId() const
{
    return FGenericTeamId(static_cast<uint8>(EMTD_TeamId::Foe));
}

void AMTD_DummyTarget::K2_OnHealthDecrease_Implementation(float LostHealth)
{
    // Empty
}

void AMTD_DummyTarget::OnHealthChanged(const FOnAttributeChangeData &ChangeData)
{
    // Notify blueprints about the delta
    K2_OnHealthDecrease(ChangeData.NewValue);
}
