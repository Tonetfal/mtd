#pragma once

#include "Engine/DataTable.h"
#include "Inventory/Items/MTD_InventoryItemsCoreTypes.h"
#include "mtd.h"

#include "MTD_BaseInventoryItemData.generated.h"

class AMTD_InventoryItemInstance;

/**
 * Base inventory item data row containing common items information.
 *
 * Specific items are intended to be created on runtime, during the gameplay. However, some templates are needed to do
 * so. It's intended to describe the way an item looks like, its meta information, and such data, without directly
 * touching any of the attributes.
 */
USTRUCT(BlueprintType)
struct FMTD_BaseInventoryItemDataRow
    : public FTableRowBase
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */

    /** Type the item is associated with. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_InventoryItemType> ItemType = EMTD_InventoryItemType::III_Invalid;
    
    /**
     * Visuals.
     */

    /** Custom inventory item instance actor class to spawn when dropped. If not set, the default one will be set. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TSubclassOf<AMTD_InventoryItemInstance> InventoryItemInstanceActorClass = nullptr;

    /** Default static mesh to use on item instance. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UStaticMesh> StaticMesh = nullptr;
    
    /** Default thumbnail to use on UI. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UTexture2D> Thumbnail = nullptr;

    /**
     * Meta information.
     */

    /** Default item name. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Name;
    
    /** Item description. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Description;
};

/**
 * Base inventory item data containing common item information.
 *
 * It's intended to save in order to use throughout different game sessions.
 */
UCLASS(Abstract, BlueprintType)
class MTD_API UMTD_BaseInventoryItemData
    : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */

    /** ID item is associated with. */
    int32 ItemID = 0;
    
    /** Type the item is associated with. Used to determine which class to cast the base item data to. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_InventoryItemType> ItemType = EMTD_InventoryItemType::III_Invalid;
    
    /**
     * Visuals.
     */

    /** Custom inventory item instance actor class to spawn when dropped. If not set, the default one will be set. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TSubclassOf<AMTD_InventoryItemInstance> InventoryItemInstanceActorClass = nullptr;

    /** Default static mesh to use on item instance. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UStaticMesh> StaticMesh = nullptr;
    
    /** Default thumbnail to use on UI. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UTexture2D> Thumbnail = nullptr;

    /**
     * Meta information.
     */
    
    /** Item name. May be changed. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Name;

    /** Item description. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Description;
};
