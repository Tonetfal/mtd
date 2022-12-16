#include "Character/MTD_TowerExtensionComponent.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_Tower.h"

UMTD_TowerExtensionComponent::UMTD_TowerExtensionComponent()
{
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_TowerExtensionComponent::SetTowerData(const UMTD_TowerData *InTowerData)
{
    check(InTowerData);

    if (IsValid(TowerData))
    {
        MTDS_ERROR("Trying to set TowerData [%s] that already has valid TowerData [%s].",
            *GetNameSafe(InTowerData), *GetNameSafe(TowerData));
        return;
    }

    TowerData = InTowerData;
}

bool UMTD_TowerExtensionComponent::IsPawnComponentReadyToInitialize() const
{
    return IsValid(TowerData);
}

void UMTD_TowerExtensionComponent::OnRegister()
{
    Super::OnRegister();

    const auto Tower = GetPawnChecked<AMTD_Tower>();
    if (!IsValid(Tower))
    {
        MTDS_ERROR("MTD_TowerExtensionComponent on [%s] can only be added to Tower actors.", *GetNameSafe(GetOwner()));
        return;
    }

    TArray<UActorComponent *> TowerExtensionComponents;
    Tower->GetComponents(StaticClass(), TowerExtensionComponents);
    if (TowerExtensionComponents.Num() != 1)
    {
        MTDS_ERROR("Only one MTD_TowerExtensionComponent should exist on [%s].", *GetNameSafe(GetOwner()));
    }
}
