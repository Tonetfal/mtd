﻿#pragma once

/**
 * Enum containing base inventory item types.
 */
UENUM(BlueprintType)
enum EMTD_InventoryItemType
{
    III_Invalid     = 0 << 0    UMETA(DisplayName="Invalid"),
    III_Armor       = 1 << 0    UMETA(DisplayName="Armor"),
    III_Weapon      = 1 << 1    UMETA(DisplayName="Weapon"),
    III_Material    = 1 << 2    UMETA(DisplayName="Material"),

    III_Equippable   = III_Armor | III_Weapon UMETA(Hidden),
};

/**
 * Enum containing weapon types.
 */
UENUM(BlueprintType)
enum EMTD_WeaponType
{
    IWT_Invalid     = 0 << 0    UMETA(DisplayName="Invalid"),
    IWT_Sword       = 1 << 0    UMETA(DisplayName="Sword"),
    IWT_Staff       = 1 << 1    UMETA(DisplayName="Staff"),

    IWT_Melee       = IWT_Sword UMETA(DisplayName="Melee"),
    IWT_Ranged      = IWT_Staff UMETA(DisplayName="Ranged"),
    IWT_Combined    = IWT_Melee | IWT_Ranged UMETA(DisplayName="Combined"),
};

/**
 * Enum containing armor types.
 */
UENUM(BlueprintType)
enum EMTD_ArmorType
{
    Invalid,
    Helmet,
    Chestplate,
    Gauntlets,
    Boots,
};
