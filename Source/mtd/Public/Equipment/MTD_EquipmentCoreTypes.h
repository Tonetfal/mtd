#pragma once

#include "Inventory/Items/MTD_InventoryItemsCoreTypes.h"
#include "mtd.h"

UENUM(BlueprintType)
enum class EMTD_EquipmentType : uint8
{
    Invalid,
    
    Helmet      = EMTD_InventoryArmorType::Helmet,
    Chestplate  = EMTD_InventoryArmorType::Chestplate,
    Gauntlets   = EMTD_InventoryArmorType::Gauntlets,
    Boots       = EMTD_InventoryArmorType::Boots,

    Weapon,
    
    Count       UMETA(Hidden)
};
