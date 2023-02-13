#include "UI/MTD_AbilityBarWidget.h"
#include "UI/MTD_UiCoreTypes.h"

#include "Character/MTD_BaseCharacter.h"

bool UMTD_AbilityBarWidget::Initialize()
{
    SelectHeroUiData();
    return Super::Initialize();
}

void UMTD_AbilityBarWidget::SelectHeroUiData()
{
    if (!IsValid(HeroesAbilityUiData))
    {
        MTDS_WARN("Heroes ability UI data is invalid");
        return;
    }

    const APawn *Player = GetOwningPlayerPawn();
    if (!IsValid(Player))
    {
        return;
    }

    for (auto &Pair :HeroesAbilityUiData->Data)
    {
        if (!Player->IsA(Pair.Key))
        {
            continue;
        }

        ActiveHeroUiData = Pair.Value;
        break;
    }

    if (!IsValid(ActiveHeroUiData))
    {
        MTDS_WARN("There is no entry for player [%s] inside heroes ability UI data", *Player->GetName());
    }
}
