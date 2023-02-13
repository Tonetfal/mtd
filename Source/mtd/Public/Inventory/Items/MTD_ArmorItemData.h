#pragma once

#include "mtd.h"
#include "MTD_EquipItemData.h"
#include "Inventory/Items/MTD_InventoryItemsCoreTypes.h"

#include "MTD_ArmorItemData.generated.h"

/**
 * Armor item data row containing generic armor information.
 */
USTRUCT(BlueprintType)
struct FMTD_ArmorItemDataRow
    : public FTableRowBase
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */

    /** Type armor is associated with. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_ArmorType> ArmorType = EMTD_ArmorType::Invalid;

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
     * Limits.
     */

    // @todo implement different attribute limits
};

/**
 * Simple structure containing all armor generated data.
 */
USTRUCT(BlueprintType)
struct FMTD_ArmorItemParameters
{
    GENERATED_BODY()

public:
    /** Resistance against physical damage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 PhysicalResistance = 0;
    
    /** Resistance against fire damage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 FireResistance = 0;

    /** Resistance against dark damage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 DarkResistance = 0;

    /** Resistance against status damage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 StatusResistance = 0;
};

/**
 * Armor item data containing specific armor information.
 */
UCLASS()
class MTD_API UMTD_ArmorItemData
    :  public UMTD_EquipItemData
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    /** Type armor is associated with. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_ArmorType> ArmorType = EMTD_ArmorType::Invalid;

    // @todo use resistance in actual computitions
    /** Item generated parameters. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMTD_ArmorItemParameters ArmorItemParameters;
};
