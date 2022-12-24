#include "UI/MTD_AbilityBarWidget.h"
#include "UI/MTD_UiCoreTypes.h"

#include "Character/MTD_BaseCharacter.h"

bool UMTD_AbilityBarWidget::Initialize()
{
    PickHeroUiData();
    return Super::Initialize();
}

void UMTD_AbilityBarWidget::PickHeroUiData()
{
    if (!IsValid(HeroesAbilityUiData))
    {
        MTDS_WARN("Heroes Ability UI Data is invalid");
        return;
    }

    APawn *Player = GetOwningPlayerPawn();
    if (!IsValid(Player))
    {
        return;
    }

    const TSubclassOf<UClass> PlayerClass = Player->GetClass();
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
        MTDS_WARN("There is no entry for player [%s] inside Heroes Ability UI Data", *GetNameSafe(Player));
    }
}
