#include "Inventory/Items/MTD_InventoryItemSpawner.h"

#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_EquipItemData.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Inventory/MTD_InventoryItemInstance.h"

AMTD_InventoryItemSpawner::AMTD_InventoryItemSpawner()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMTD_InventoryItemSpawner::OnConstruction(const FTransform &Transform)
{
    Super::OnConstruction(Transform);

    // Destroy a previously created item instance
    if (IsValid(ItemInstance))
    {
        ItemInstance->Destroy();
    }

    // Create logical item
    UMTD_BaseInventoryItemData *ItemData =
        UMTD_InventoryBlueprintFunctionLibrary::CreateBaseInventoryItemData(this, ItemID);
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Failed to generate an inventory data asset with item ID [%d].", ItemID);
        return;
    }
    
    if (ItemData->InventoryItemInstanceActorClass)
    {
        // Use a custom one
        InventoryItemInstanceClass = ItemData->InventoryItemInstanceActorClass;
    }
    
    if (!InventoryItemInstanceClass)
    {
        MTDS_WARN("Inventory item instance class is invalid.");
        return;
    }

    // Use parameters set prior to runtime
    DispatchItemParameters(ItemData);

    UWorld *World = GetWorld();
    if (!IsValid(World))
    {
        MTDS_WARN("World is invalid.");
        return;
    }
    
    constexpr auto CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    // Create physical item
    ItemInstance = World->SpawnActorDeferred<AMTD_InventoryItemInstance>(
        InventoryItemInstanceClass, Transform, nullptr, nullptr, CollisionHandlingMethod);
    
    if (!IsValid(ItemInstance))
    {
        MTDS_WARN("Failed to spawn item instance.");
        return;
    }

    // Set item data before running initialization on item itself
    ItemInstance->SetItemData(ItemData);
    
    // Finish spawning
    ItemInstance->FinishSpawning(Transform);
}

void AMTD_InventoryItemSpawner::BeginPlay()
{
    Super::BeginPlay();

    // The purpose is finished
    Destroy();
}

void AMTD_InventoryItemSpawner::DispatchItemParameters(UMTD_BaseInventoryItemData *ItemData) const
{
    check(IsValid(ItemData));

    switch (ItemData->ItemType)
    {
    case EMTD_InventoryItemType::III_Armor:
        DispatchEquipParameters(ItemData);
        DispatchArmorParameters(ItemData);
        break;
    case EMTD_InventoryItemType::III_Weapon:
        DispatchEquipParameters(ItemData);
        DispatchWeaponParameters(ItemData);
        break;
    default:
        break;
    }
}

void AMTD_InventoryItemSpawner::DispatchEquipParameters(UMTD_BaseInventoryItemData *ItemData) const
{
    auto EquippableItemData = CastChecked<UMTD_EquipItemData>(ItemData);
    EquippableItemData->EquippableItemParameters = EquipItemParameters;
}

void AMTD_InventoryItemSpawner::DispatchArmorParameters(UMTD_BaseInventoryItemData *ItemData) const
{
    auto ArmorItemData = CastChecked<UMTD_ArmorItemData>(ItemData);
    ArmorItemData->ArmorItemParameters = ArmorItemParameters;
}

void AMTD_InventoryItemSpawner::DispatchWeaponParameters(UMTD_BaseInventoryItemData *ItemData) const
{
    auto WeaponItemData = CastChecked<UMTD_WeaponItemData>(ItemData);
    WeaponItemData->WeaponItemParameters = WeaponItemParameters;
}
