#include "Character/MTD_EnemyExtensionComponent.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_BaseEnemyCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_PawnExtensionComponent.h"

UMTD_EnemyExtensionComponent::UMTD_EnemyExtensionComponent()
{
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_EnemyExtensionComponent::SetEnemyData(const UMTD_EnemyData *InEnemyData)
{
    check(InEnemyData);

    if (IsValid(EnemyData))
    {
        MTDS_ERROR("Trying to set EnemyData [%s] that already has valid EnemyData [%s].",
            *GetNameSafe(InEnemyData), *GetNameSafe(EnemyData));
        return;
    }

    EnemyData = InEnemyData;
}

bool UMTD_EnemyExtensionComponent::IsPawnComponentReadyToInitialize() const
{
    return IsValid(EnemyData);
}

void UMTD_EnemyExtensionComponent::OnRegister()
{
    Super::OnRegister();

    const auto Enemy = GetPawnChecked<AMTD_BaseEnemyCharacter>();
    if (!IsValid(Enemy))
    {
        MTDS_ERROR("MTD_EnemyExtensionComponent on [%s] can only be added to Enemy actors.", *GetNameSafe(GetOwner()));
        return;
    }

    TArray<UActorComponent *> EnemyExtensionComponents;
    Enemy->GetComponents(StaticClass(), EnemyExtensionComponents);
    if (EnemyExtensionComponents.Num() != 1)
    {
        MTDS_ERROR("Only one MTD_EnemyExtensionComponent should exist on [%s].", *GetNameSafe(GetOwner()));
    }
}
