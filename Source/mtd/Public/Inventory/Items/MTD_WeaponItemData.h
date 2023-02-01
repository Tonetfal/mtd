#pragma once

#include "mtd.h"
#include "MTD_EquippableItemData.h"
#include "Inventory/Items/MTD_InventoryItemsCoreTypes.h"

#include "MTD_WeaponItemData.generated.h"

class UMTD_EquipmentDefinitionAsset;

USTRUCT(BlueprintType)
struct FMTD_WeaponItemDataRow
    : public FTableRowBase
{
    GENERATED_BODY()

public:
    inline static constexpr float InvalidLimitValue = -1.f;

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_InventoryWeaponType> WeaponType = EMTD_InventoryWeaponType::IWT_Invalid;
    
    /** Hero classes that can use the item. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    FGameplayTagContainer HeroClasses;

    /**
     * Visuals.
     */

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UMTD_EquipmentDefinitionAsset> EquipmentDefinitionAsset = nullptr;

    /**
     * Limits.
     */

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute Limits")
    int32 ProjectilesLimit = 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute Limits")
    float ProjectileSpeedLimit = 100000.f;
};

UCLASS()
class MTD_API UMTD_WeaponItemData
    :  public UMTD_EquippableItemData
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_InventoryWeaponType> WeaponType = EMTD_InventoryWeaponType::IWT_Invalid;

    /**
     * Weapon attributes.
     */

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Weapon")
    int32 MeleeDamage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    int32 RangedDamage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    int32 Projectiles = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    float ProjectileSpeed = 0.f;
};
