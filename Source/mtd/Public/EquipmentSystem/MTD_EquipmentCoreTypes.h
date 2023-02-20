#pragma once

#include "InventorySystem/Items/MTD_InventoryItemsCoreTypes.h"
#include "mtd.h"

/**
 * Enum to identify all equippable types.
 *
 * Player character must be able to equip all of the at once.
 */
UENUM(BlueprintType)
enum class EMTD_EquipmentType : uint8
{
    Invalid,
    
    Helmet      = EMTD_ArmorType::Helmet,
    Chestplate  = EMTD_ArmorType::Chestplate,
    Gauntlets   = EMTD_ArmorType::Gauntlets,
    Boots       = EMTD_ArmorType::Boots,

    Weapon,
    
    Count       UMETA(Hidden)
};
