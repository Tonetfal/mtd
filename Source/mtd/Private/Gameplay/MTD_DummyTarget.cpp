#include "Gameplay/MTD_DummyTarget.h"

#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "MTD_CoreTypes.h"
#include "System/MTD_Tags.h"

AMTD_DummyTarget::AMTD_DummyTarget()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    AbilitySystemComponent = CreateDefaultSubobject<UMTD_AbilitySystemComponent>("MTD Ability System Component");
    
    UMTD_HealthSet *HealthSet = CreateDefaultSubobject<UMTD_HealthSet>("Health Set");
    AbilitySystemComponent->AddSpawnedAttribute(HealthSet);

    Tags.Add(FMTD_Tags::Enemy);
}

void AMTD_DummyTarget::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_HealthSet::GetLastLostHealth_MetaAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

UAbilitySystemComponent *AMTD_DummyTarget::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

FGenericTeamId AMTD_DummyTarget::GetGenericTeamId() const
{
    return FGenericTeamId(static_cast<uint8>(EMTD_TeamId::Enemy));
}

void AMTD_DummyTarget::K2_OnHealthDecrease_Implementation(float LostHealth)
{
    // Empty
}

void AMTD_DummyTarget::OnHealthChanged(const FOnAttributeChangeData &Attribute)
{
    K2_OnHealthDecrease(Attribute.NewValue);
}
