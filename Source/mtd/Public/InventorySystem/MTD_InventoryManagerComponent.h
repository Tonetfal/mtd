#pragma once

#include "Components/ActorComponent.h"
#include "mtd.h"

#include "MTD_InventoryManagerComponent.generated.h"

class AMTD_BaseCharacter;
class AMTD_InventoryItemInstance;
class UMTD_BaseInventoryItemData;
class UMTD_MaterialItemData;

/**
 * Enum to indicate result of adding an item to the inventory.
 */
UENUM(BlueprintType)
enum class EMTD_InventoryResult : uint8
{
    Added,
    Dropped,
    Failed
};

/**
 * Component to store all items, as well as manage their actions.
 */
UCLASS(BlueprintType, ClassGroup=("Character"), meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_InventoryManagerComponent
    : public UActorComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemAddedSignature,
        UMTD_BaseInventoryItemData *, ItemData);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemRemovedSignature,
        UMTD_BaseInventoryItemData *, ItemData);

public:
    UMTD_InventoryManagerComponent();
    
    /**
     * Find inventory manager component on a given actor.
     * @param   Actor: actor to search for inventory manager component in.
     * @result  Inventory manager component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Inventory Manager Component")
    static UMTD_InventoryManagerComponent *FindInventoryManagerComponent(const AActor *Actor);

    /**
     * Check whether it's possible to add an item.
     * @param   ItemData: item to check.
     * @return  If true, item can be added, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Inventory Manager Component")
    bool CanAddItem(const UMTD_BaseInventoryItemData *ItemData) const;

    /**
     * Try to add an item.
     * @param   ItemData: item to add.
     * @param   bDropIfNotEnoughSpace: if true, in case of fail, an instance of item will be created instead, so that it
     * can be picked up. In this case item data ownership is not client's anymore. If false, in case of fail nothing
     * will be changed.
     * @return  Result of trying to add an item to the inventory.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Inventory Manager Component")
    EMTD_InventoryResult AddItem(UMTD_BaseInventoryItemData *ItemData, bool bDropIfNotEnoughSpace = false);

    /**
     * Try to remove an item from inventory.
     * @param   ItemData: item to remove.
     * @return  If true, item has successfully been removed, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Inventory Manager Component")
    bool RemoveItem(UMTD_BaseInventoryItemData *ItemData);

    /**
     * Try to drop an item from inventory. If run successfully, item will be removed from inventory, and a valid
     * inventory item instance will be returned.
     * @param   ItemData: item to drop.
     * @return  Inventory item instance associated with removed item data.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Inventory Manager Component")
    AMTD_InventoryItemInstance *DropItem(UMTD_BaseInventoryItemData *ItemData);

    /**
     * Get maximum amount of slots.
     * @return  Maximum amount of slots.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Inventory Manager Component")
    int32 GetMaxSlots() const;
    
    /**
     * Get amount of used slots.
     * @return  Amount of used slots.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Inventory Manager Component")
    int32 GetUsedSlots() const;
    
    /**
     * Get amount of spare slots.
     * @return  Spare of used slots.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Inventory Manager Component")
    int32 GetRemainingSlots() const;

    /**
     * Check whether the inventory is full.
     * @return  If true, there is no more slots, false otherwise. Note that some items may still fit regardless, if
     * there are stackable, and there are stacks they fit into. Use CanAddItem for that case.
     *
     * @see CanAddItem
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Inventory Manager Component")
    bool IsFull() const;

    /**
     * Check whether the inventory is empty or not.
     * @return  If true, all slots are empty, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Inventory Manager Component")
    bool IsEmpty() const;

private:
    /**
     * Add weapon item.
     * @param   ItemData: weapon item data.
     */
    void AddWeaponItem(UMTD_BaseInventoryItemData *ItemData);
    
    /**
     * Add armor item.
     * @param   ItemData: armor item data.
     */
    void AddArmorItem(UMTD_BaseInventoryItemData *ItemData);
    
    /**
     * Add matierla item.
     * @param   ItemData: material item data.
     */
    void AddMaterialItem(UMTD_BaseInventoryItemData *ItemData);

    /**
     * Find all item entries for a given ID.
     * @param   ItemID: item identifier to search for.
     * @return  Container of items with the specified ID.
     */
    TArray<const UMTD_BaseInventoryItemData *> FindItemsOfID(int32 ItemID) const;
    
    /**
     * Find all item entries for a given ID.
     * @param   ItemID: item identifier to search for.
     * @return  Container of items with the specified ID.
     */
    TArray<UMTD_BaseInventoryItemData *> FindItemsOfID(int32 ItemID);

    /**
     * Find the first entry of an incomplete stackable item. The entry will be able to hold at least 1 more item.
     * @param   ItemID: item identifier to search for.
     * @return  Incomplete stackable item entry. May be nullptr.
     */
    const UMTD_MaterialItemData *FindIncompleteStackableItemEntryOfID(int32 ItemID) const;
    
    /**
     * Find the first entry of an incomplete stackable item. The entry will be able to hold at least 1 more item.
     * @param   ItemID: item identifier to search for.
     * @return  Incomplete stackable item entry. May be nullptr.
     */
    UMTD_MaterialItemData *FindIncompleteStackableItemEntryOfID(int32 ItemID);

    /**
     * Find the first item entry for a given ID.
     * @param   ItemID: item identifier to search for.
     * @return  Item entry with the specified ID. May be nullptr.
     */
    const UMTD_BaseInventoryItemData *FindItemOfID(int32 ItemID) const;
    
    /**
     * Find the first item entry for a given ID.
     * @param   ItemID: item identifier to search for.
     * @return  Item entry with the specified ID. May be nullptr.
     */
    UMTD_BaseInventoryItemData *FindItemOfID(int32 ItemID);

    /**
     * Check whether item with given ID is present.
     * @param   ItemID: item identifier to search for.
     * @return  If true, item is present, false otherwise.
     */
    bool ContainsItemID(int32 ItemID) const;
    
    /**
     * Check whether item with given ID can be stacked on an already present stack.
     * @param   ItemID: item identifier to search for.
     * @return  If true, there is a spare item stack to add items to, false otherwise.
     */
    bool CanStackItem(int32 ItemID) const;

    /**
     * Get physical character containing the inventory.
     * @return  Physical character containing the inventory. May be nullptr.
     */
    AMTD_BaseCharacter *GetCharacter() const;

public:
    /** Delegate to fire when item is added. */
    UPROPERTY(BlueprintAssignable)
    FOnItemAddedSignature OnItemAddedDelegate;
    
    /** Delegate to fire when item is removed .*/
    UPROPERTY(BlueprintAssignable)
    FOnItemRemovedSignature OnItemRemovedDelegate;

private:
    /** Maximum capacity of the inventory. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Inventory Manager Component", meta=(ClampMin="0.0"))
    int32 MaxSlots = 25;

    /** Stored item entries. */
    UPROPERTY(BlueprintReadOnly, Category="MTD|Inventory Manager Component", meta=(AllowPrivateAccess="true"))
    TArray<TObjectPtr<UMTD_BaseInventoryItemData>> InventoryEntries;
};

inline UMTD_InventoryManagerComponent *UMTD_InventoryManagerComponent::FindInventoryManagerComponent(
    const AActor *Actor)
{
    return ((IsValid(Actor) ? (Actor->FindComponentByClass<UMTD_InventoryManagerComponent>()) : (nullptr)));
}
