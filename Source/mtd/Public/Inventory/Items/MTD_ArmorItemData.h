#pragma once

#include "mtd.h"
#include "MTD_EquippableItemData.h"
#include "Inventory/Items/MTD_InventoryItemsCoreTypes.h"

#include "MTD_ArmorItemData.generated.h"

USTRUCT(BlueprintType)
struct FMTD_ArmorItemDataRow
    : public FTableRowBase
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    EMTD_InventoryArmorType ArmorType = EMTD_InventoryArmorType::Invalid;

    /** Hero classes that can use the item. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    FGameplayTagContainer HeroClasses;

    /**
     * Limits.
     */

    // @todo implement different attribute limits
};

UCLASS()
class MTD_API UMTD_ArmorItemData
    :  public UMTD_EquippableItemData
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    EMTD_InventoryArmorType ArmorType = EMTD_InventoryArmorType::Invalid;

    /**
     * Armor attributes.
     */

    // @todo use resistance in actual computitions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Armor")
    int32 PhysicalResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Armor")
    int32 FireResistance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Armor")
    int32 DarkResistance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Armor")
    int32 StatusResistance = 0;
};
