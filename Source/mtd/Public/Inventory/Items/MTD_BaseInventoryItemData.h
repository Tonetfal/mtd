#pragma once

#include "Engine/DataTable.h"
#include "Inventory/Items/MTD_InventoryItemsCoreTypes.h"
#include "mtd.h"

#include "MTD_BaseInventoryItemData.generated.h"

class AMTD_InventoryItemInstance;

USTRUCT(BlueprintType)
struct FMTD_BaseInventoryItemDataRow
    : public FTableRowBase
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_InventoryItemType> ItemType = EMTD_InventoryItemType::III_Invalid;
    
    /**
     * Visuals.
     */

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TSubclassOf<AMTD_InventoryItemInstance> InventoryItemInstanceActorClass = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UStaticMesh> StaticMesh = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UTexture2D> Thumbnail = nullptr;

    /**
     * Meta information.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Name;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Description;
};

UCLASS(Abstract, BlueprintType)
class MTD_API UMTD_BaseInventoryItemData
    : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */

    int32 ItemID = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    TEnumAsByte<EMTD_InventoryItemType> ItemType = EMTD_InventoryItemType::III_Invalid;
    
    /**
     * Visuals.
     */

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TSubclassOf<AMTD_InventoryItemInstance> InventoryItemInstanceActorClass = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UStaticMesh> StaticMesh = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UTexture2D> Thumbnail = nullptr;

    /**
     * Meta information.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Name;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Description")
    FText Description;
};
