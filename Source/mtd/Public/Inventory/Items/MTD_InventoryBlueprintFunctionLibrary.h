#pragma once

#include "Equipment/MTD_EquipmentCoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "mtd.h"

#include "MTD_InventoryBlueprintFunctionLibrary.generated.h"

class AMTD_BaseFoeCharacter;
class AMTD_InventoryItemInstance;
class UMTD_BaseInventoryItemData;

/**
 * Utility functions to work with inventory items.
 */
UCLASS()
class MTD_API UMTD_InventoryBlueprintFunctionLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    /**
     * Generate a logical item data. Stats are generated randomly.
     * @param   WorldContextObject: context the function is called with.
     * @param   ItemID: item idenfitier that has to spawn.
     * @param   Difficulty: difficulty the game is on. Key factor for quality.
     * @return  Generated inventory item data. May be nullptr.
     */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static UMTD_BaseInventoryItemData *GenerateInventoryItemData(
        UObject *WorldContextObject, int32 ItemID, float Difficulty);

    /**
     * Generate a logical item data that is supposed to be dropped by a foe. Stats are generated randomly.
     * @param   WorldContextObject: context the function is called with.
     * @return  Generate inventory item data. May be nullptr.
     */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static UMTD_BaseInventoryItemData *GenerateDropItem(AMTD_BaseFoeCharacter *WorldContextObject);
    
    /**
     * Generate an item (both item data and item instance) that is supposed to be dropped by a foe. Stats are generated
     * randomly.
     * @param   WorldContextObject: context the function is called with.
     * @param   bWarnIfFailed: if true, a warning in log is printed when fails, if false, nothing will be printed.
     * @return  Generate item to drop. May be nullptr.
     */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AMTD_InventoryItemInstance *GenerateDropItemInstance(AMTD_BaseFoeCharacter *WorldContextObject,
        bool bWarnIfFailed = true);

    /**
     * Create a logical item data. Stats will be at their default values.
     * @param   WorldContextObject: context the function is called with.
     * @param   ItemID: item idenfitier that has to spawn.
     * @return  Created inventory item data. May be nullptr.
     */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static UMTD_BaseInventoryItemData *CreateBaseInventoryItemData(UObject* WorldContextObject, int32 ItemID);

    /**
     * Create a physical item instance. Wrap backend item data with some frontend.
     * @param   WorldContextObject: context the function is called with.
     * @param   ItemData: item data to use for the created item instance. If item instance has been successfully
     * created, the ownership is passed to it.
     * @return  Item instance. May be nullptr.
     */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AMTD_InventoryItemInstance *CreateItemInstance(AActor* WorldContextObject,
        UMTD_BaseInventoryItemData *ItemData);

    /**
     * Retrieve equipment type from base inventory item data.
     * @param   ItemData: item data to retrieve equipment type from.
     * @return  Equipment type.
     */
	UFUNCTION(BlueprintCallable, BlueprintPure)
    static EMTD_EquipmentType GetEquipmentType(const UMTD_BaseInventoryItemData *ItemData);

    /**
     * Check whether item is an equippable.
     * @param   ItemData: item to check.
     * @return  If true, item can be equipped, false otherwise.
     */
	UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEquippable(const UMTD_BaseInventoryItemData *ItemData);

    /**
     * Get amount of copies in stack of a given item.
     * @param   ItemData: item to check.
     * @return  Amount of copies in stack. If fails, -1 is returned to indicate the error.
     */
	UFUNCTION(BlueprintCallable, BlueprintPure)
    static int32 GetItemAmount(const UMTD_BaseInventoryItemData *ItemData);

    /**
     * Get maximum amount of copies in stack of a given item.
     * @param   ItemData: item to check.
     * @return  Maximum amount of copies in stack. If fails, -1 is returned to indicate the error.
     */
	UFUNCTION(BlueprintCallable, BlueprintPure)
    static int32 GetMaxItemAmount(const UMTD_BaseInventoryItemData *ItemData);
};
