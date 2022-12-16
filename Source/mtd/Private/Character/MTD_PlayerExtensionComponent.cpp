#include "Character/MTD_PlayerExtensionComponent.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"

UMTD_PlayerExtensionComponent::UMTD_PlayerExtensionComponent()
{
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_PlayerExtensionComponent::SetPlayerData(const UMTD_PlayerData *InPlayerData)
{
    check(InPlayerData);

    if (IsValid(PlayerData))
    {
        MTDS_ERROR("Trying to set PlayerData [%s] that already has valid PlayerData [%s].",
            *GetNameSafe(InPlayerData), *GetNameSafe(PlayerData));
        return;
    }

    PlayerData = InPlayerData;
}

bool UMTD_PlayerExtensionComponent::IsPawnComponentReadyToInitialize() const
{
    return IsValid(PlayerData);
}

void UMTD_PlayerExtensionComponent::OnRegister()
{
    Super::OnRegister();

    const auto Player = GetPawnChecked<AMTD_BasePlayerCharacter>();
    if (!IsValid(Player))
    {
        MTDS_ERROR("MTD_PlayerExtensionComponent on [%s] can only be added to Player actors.", *GetNameSafe(GetOwner()));
        return;
    }

    TArray<UActorComponent *> PlayerExtensionComponents;
    Player->GetComponents(StaticClass(), PlayerExtensionComponents);
    if (PlayerExtensionComponents.Num() != 1)
    {
        MTDS_ERROR("Only one MTD_PlayerExtensionComponent should exist on [%s].", *GetNameSafe(GetOwner()));
    }
}
