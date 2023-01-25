#include "Inventory/Items/MTD_InventoryItemSpawner.h"

#include "Inventory/MTD_InventoryItemInstance.h"
#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_EquippableItemData.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Kismet/GameplayStatics.h"

AMTD_InventoryItemSpawner::AMTD_InventoryItemSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMTD_InventoryItemSpawner::PreInitializeComponents()
{
    Super::PreInitializeComponents();
    
    UMTD_BaseInventoryItemData *ItemData =
        UMTD_InventoryBlueprintFunctionLibrary::CreateBaseInventoryItemData(this, ItemID);

    if (!IsValid(ItemData))
    {
        MTDS_WARN("Failed to generate an Inventory Data Asset with ID [%d].", ItemID);
        return;
    }
    
    // Try to override
    if (ItemData->InventoryItemInstanceActorClass)
    {
        InventoryItemInstanceClass = ItemData->InventoryItemInstanceActorClass;
    }
    
    if (!InventoryItemInstanceClass)
    {
        MTDS_WARN("Inventory Item Instance Class is invalid.");
        return;
    }

    // Use parameters set prior to run-time
    DispatchItemParameters(ItemData);

    // Spawn the item instance
    const FTransform Transform = GetActorTransform();
    auto ItemInstance = GetWorld()->SpawnActorDeferred<AMTD_InventoryItemInstance>(
        InventoryItemInstanceClass, Transform, nullptr, nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    // Finilize the item instance
    ItemInstance->SetItemData(ItemData);
    UGameplayStatics::FinishSpawningActor(ItemInstance, Transform);
}

void AMTD_InventoryItemSpawner::BeginPlay()
{
    Super::BeginPlay();

    // The purpose is finished
    Destroy();
}

void AMTD_InventoryItemSpawner::DispatchItemParameters(UMTD_BaseInventoryItemData *ItemData)
{
    check(ItemData);

    switch (ItemData->ItemType)
    {
    case EMTD_InventoryItemType::III_Armor:
        DispatchEquippableParameters(ItemData);
        DispatchArmorParameters(ItemData);
        break;
    case EMTD_InventoryItemType::III_Weapon:
        DispatchEquippableParameters(ItemData);
        DispatchWeaponParameters(ItemData);
        break;
    default:
        break;
    }
    
}

void AMTD_InventoryItemSpawner::DispatchEquippableParameters(UMTD_BaseInventoryItemData *ItemData)
{
    auto EquippableItemData = Cast<UMTD_EquippableItemData>(ItemData);
    check(EquippableItemData);

    EquippableItemData->LevelRequired = EquippableItemParameters.LevelRequired;
    EquippableItemData->PlayerHealth = EquippableItemParameters.PlayerHealth;
    EquippableItemData->PlayerDamage = EquippableItemParameters.PlayerDamage;
    EquippableItemData->PlayerSpeed = EquippableItemParameters.PlayerSpeed;
    EquippableItemData->TowerHealth = EquippableItemParameters.TowerHealth;
    EquippableItemData->TowerDamage = EquippableItemParameters.TowerDamage;
    EquippableItemData->TowerRange = EquippableItemParameters.TowerRange;
    EquippableItemData->TowerSpeed = EquippableItemParameters.TowerSpeed;
}

void AMTD_InventoryItemSpawner::DispatchArmorParameters(UMTD_BaseInventoryItemData *ItemData)
{
    auto ArmorItemData = Cast<UMTD_ArmorItemData>(ItemData);
    check(ArmorItemData);

    ArmorItemData->PhysicalResistance = ArmorItemParameters.PhysicalResistance;
    ArmorItemData->FireResistance = ArmorItemParameters.FireResistance;
    ArmorItemData->DarkResistance = ArmorItemParameters.DarkResistance;
    ArmorItemData->StatusResistance = ArmorItemParameters.StatusResistance;
}

void AMTD_InventoryItemSpawner::DispatchWeaponParameters(UMTD_BaseInventoryItemData *ItemData)
{
    auto WeaponItemData = Cast<UMTD_WeaponItemData>(ItemData);
    check(WeaponItemData);

    WeaponItemData->MeleeDamage = WeaponItemParameters.MeleeDamage;
    WeaponItemData->BalanceDamage = WeaponItemParameters.BalanceDamage;
    WeaponItemData->RangedDamage = WeaponItemParameters.RangedDamage;
    WeaponItemData->Projectiles = WeaponItemParameters.Projectiles;
    WeaponItemData->ProjectileSpeed = WeaponItemParameters.ProjectileSpeed;
}
