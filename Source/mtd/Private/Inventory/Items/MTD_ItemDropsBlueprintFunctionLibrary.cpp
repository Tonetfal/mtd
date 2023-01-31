#include "Inventory/Items/MTD_ItemDropsBlueprintFunctionLibrary.h"

#include "Character/MTD_BaseEnemyCharacter.h"
#include "GameModes/MTD_TowerDefenseMode.h"
#include "Inventory/MTD_InventoryItemInstance.h"
#include "Inventory/Items/MTD_BaseInventoryItemData.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Inventory/Items/MTD_ItemDropManager.h"
#include "Kismet/GameplayStatics.h"

UMTD_BaseInventoryItemData *UMTD_ItemDropsBlueprintFunctionLibrary::CreateRandomDropItem(
    AMTD_BaseEnemyCharacter *WorldContextObject)
{
    if (!IsValid(WorldContextObject))
    {
        MTD_WARN("World Context Object is invalid.");
        return nullptr;
    }

    const UMTD_ItemDropManager *ItemDrops = UMTD_ItemDropManager::Get();
    if (!IsValid(ItemDrops))
    {
        MTD_WARN("Item Drops is invalid.");
        return nullptr;
    }

    const auto Tdm = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(WorldContextObject->GetWorld()));
    if (!IsValid(Tdm))
    {
        MTD_WARN("Tower Defense Mode is invalid.");
        return nullptr;
    }
    
    int32 ItemID = 0;
    const TSubclassOf<AMTD_BaseEnemyCharacter> CharacterClass = WorldContextObject->GetClass();
    if (!ItemDrops->GetRandomItemID(CharacterClass, ItemID))
    {
        return nullptr;
    }
    
    const float Difficulty = Tdm->GetScaledDifficulty();
    UMTD_BaseInventoryItemData *ItemData = UMTD_InventoryBlueprintFunctionLibrary::GenerateInventoryItemData(
        WorldContextObject, ItemID, Difficulty);

    return ItemData;
}

AMTD_InventoryItemInstance *UMTD_ItemDropsBlueprintFunctionLibrary::CreateRandomDropItemInstance(
    AMTD_BaseEnemyCharacter *WorldContextObject, bool bWarnIfFailed)
{
    UMTD_BaseInventoryItemData *ItemData = CreateRandomDropItem(WorldContextObject);
    if (!IsValid(ItemData))
    {
        if (bWarnIfFailed)
        {
            MTD_WARN("Item Data is invalid.");
        }
        return nullptr;
    }

    AMTD_InventoryItemInstance *ItemInstance = UMTD_InventoryBlueprintFunctionLibrary::CreateItemInstance(
        WorldContextObject, ItemData);

    return ItemInstance;
}
