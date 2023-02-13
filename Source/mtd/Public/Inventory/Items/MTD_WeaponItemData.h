#pragma once

#include "mtd.h"
#include "MTD_EquipItemData.h"
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

    /** Type weapon is associated with. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_WeaponType> WeaponType = EMTD_WeaponType::IWT_Invalid;
    
    /** Hero classes that can use the item. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    FGameplayTagContainer HeroClasses;

    /**
     * Visuals.
     */

    /** Custom visual representation of equipment on character. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UMTD_EquipmentDefinitionAsset> EquipmentDefinitionAsset = nullptr;

    /**
     * Parameters.
     */

    /**
     * If true, projectile will damage everything in a range, if false, only actors hit by the projectile hitbox will
     * receive damage.
     *
     * @see RadialRange
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Parameters")
    bool bRadial = false;

    /**
     * Limits.
     */

    /** Maximum projectiles per attack. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute Limits")
    int32 ProjectilesLimit = 1;
    
    /** Maximum projectile speed. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute Limits")
    float ProjectileSpeedLimit = 100000.f;
    
    /** Maximum range radial projectile can damage in. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute Limits")
    float RadialRangeLimit = 500.f;
};

/**
 * Simple structure containing all weapon generated data.
 */
USTRUCT(BlueprintType)
struct FMTD_WeaponItemParameters
{
    GENERATED_BODY()

public:
    /**
     * Weapon attributes.
     */

    /** Base damage to deal with melee attacks. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Weapon")
    int32 MeleeDamage = 0;

    /** Base damage to deal on projectile impact. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    int32 RangedDamage = 0;

    /** Amount of projectiles per attack. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    int32 Projectiles = 0;

    /** Speed projectile travels at. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    float ProjectileSpeed = 0.f;
    
    /** Is radial? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    bool bRadial = false;

    /** Range radial projectile damages in. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Ranged Weapon")
    float RadialRange = 0.f;
};

UCLASS()
class MTD_API UMTD_WeaponItemData
    :  public UMTD_EquipItemData
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_WeaponType> WeaponType = EMTD_WeaponType::IWT_Invalid;

    /** Item generated parameters. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMTD_WeaponItemParameters WeaponItemParameters;
};
