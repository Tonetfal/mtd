#include "InventorySystem/MTD_InventoryManagerComponent.h"

#include "Character/MTD_BasePlayerCharacter.h"
#include "InventorySystem/MTD_InventoryItemInstance.h"
#include "InventorySystem/Items/MTD_BaseInventoryItemData.h"
#include "InventorySystem/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "InventorySystem/Items/MTD_InventoryItemsCoreTypes.h"
#include "InventorySystem/Items/MTD_MaterialItemData.h"
#include "Player/MTD_PlayerState.h"

UMTD_InventoryManagerComponent::UMTD_InventoryManagerComponent()
{
    // Nothing to tick for
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UMTD_InventoryManagerComponent::CanAddItem(const UMTD_BaseInventoryItemData *ItemData) const
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item data is invalid.");
        return false;
    }
    
    switch (ItemData->ItemType)
    {
    case EMTD_InventoryItemType::III_Armor:
    case EMTD_InventoryItemType::III_Weapon:
        return (!IsFull());
    case EMTD_InventoryItemType::III_Material:
        return ((!IsFull()) ? (true) : (CanStackItem(ItemData->ItemID)));
    default:
        MTDS_WARN("Unknown item type [%s].", *UEnum::GetValueAsString(ItemData->ItemType));
        return false;
    }
}

EMTD_InventoryResult UMTD_InventoryManagerComponent::AddItem(UMTD_BaseInventoryItemData *ItemData,
    bool bDropIfNotEnoughSpace /* = false */)
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item data is invalid.");
        return EMTD_InventoryResult::Failed;
    }

    if (!CanAddItem(ItemData))
    {
        if (!bDropIfNotEnoughSpace)
        {
            MTDS_WARN("Item [%s] cannot be added to inventory.", *ItemData->GetName());
            
            return EMTD_InventoryResult::Failed;
        }
        else
        {
            MTDS_WARN("Item [%s] cannot be added to inventory. It will dropped on the floor.", *ItemData->GetName());

            // Spawn the item
            AMTD_BaseCharacter *MtdCharacter = GetCharacter();
            const AMTD_InventoryItemInstance *ItemInstance =
                UMTD_InventoryBlueprintFunctionLibrary::CreateItemInstance(MtdCharacter, ItemData);
            
            return ((IsValid(ItemInstance)) ? (EMTD_InventoryResult::Dropped) : (EMTD_InventoryResult::Failed));
        }
    }

    // Add the item
    switch (ItemData->ItemType)
    {
    case EMTD_InventoryItemType::III_Armor: 
        AddArmorItem(ItemData); 
        break;
    case EMTD_InventoryItemType::III_Weapon: 
        AddWeaponItem(ItemData); 
        break;
    case EMTD_InventoryItemType::III_Material: 
        AddMaterialItem(ItemData); 
        break;
    default:
        MTDS_WARN("Unknown item type [%s].", *UEnum::GetValueAsString(ItemData->ItemType));
        return EMTD_InventoryResult::Failed;
    }

    // Notify about the event
    OnItemAddedDelegate.Broadcast(ItemData);
    
    MTDS_LOG("Item [%s] has been added to inventory.", *ItemData->Name.ToString());
    return EMTD_InventoryResult::Added;
}

bool UMTD_InventoryManagerComponent::RemoveItem(UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item data is invalid.");
        return false;
    }

    const int32 RemovedAmount = InventoryEntries.Remove(ItemData);
    if (RemovedAmount == 0)
    {
        MTDS_WARN("Item [%s] is not present in inventory.", *ItemData->GetName());
        return false;
    }

    // Notify about the event
    OnItemRemovedDelegate.Broadcast(ItemData);

    MTDS_LOG("Item [%d] has been removed from inventory.", RemovedAmount);
    return true;
}

AMTD_InventoryItemInstance *UMTD_InventoryManagerComponent::DropItem(UMTD_BaseInventoryItemData *ItemData)
{
    // Try to remove the item
    if (!RemoveItem(ItemData))
    {
        return nullptr;
    }

    // Use character as the world context in order to spawn the item at his transforms
    AMTD_BaseCharacter *MtdCharacter = GetCharacter();

    // Spawn the item
    AMTD_InventoryItemInstance *ItemInstance = UMTD_InventoryBlueprintFunctionLibrary::CreateItemInstance(
        MtdCharacter, ItemData);

    MTDS_LOG("Item [%s] has been dropped [%s].", *ItemData->GetName(), *GetNameSafe(ItemInstance));
    return ItemInstance;
}

int32 UMTD_InventoryManagerComponent::GetMaxSlots() const
{
    return MaxSlots;
}

int32 UMTD_InventoryManagerComponent::GetUsedSlots() const
{
    const int32 UsedSlots = InventoryEntries.Num();
    return UsedSlots;
}

int32 UMTD_InventoryManagerComponent::GetRemainingSlots() const
{
    const int32 UsedSlots = GetUsedSlots();
    const int32 RemainingSlots = (MaxSlots - UsedSlots);
    return RemainingSlots;
}

bool UMTD_InventoryManagerComponent::IsFull() const
{
    const int32 RemainingSlots = GetRemainingSlots();
    return (RemainingSlots == 0);
}

bool UMTD_InventoryManagerComponent::IsEmpty() const
{
    const int32 RemainingSlots = GetRemainingSlots();
    return (RemainingSlots != 0);
}

TArray<const UMTD_BaseInventoryItemData*> UMTD_InventoryManagerComponent::FindItemsOfID(int32 ItemID) const
{
    TArray<const UMTD_BaseInventoryItemData *> Result;
    
    // Iterate through each entry and compare the item ID
    for (const UMTD_BaseInventoryItemData *ItemData : InventoryEntries)
    {
        if (IsValid(ItemData))
        {
            if (ItemData->ItemID == ItemID)
            {
                Result.Push(ItemData);
            }
        }
    }
    
    return Result;
}

TArray<UMTD_BaseInventoryItemData*> UMTD_InventoryManagerComponent::FindItemsOfID(int32 ItemID)
{
    TArray<UMTD_BaseInventoryItemData *> Result;
    
    // Iterate through each entry and compare the item ID
    for (UMTD_BaseInventoryItemData *ItemData : InventoryEntries)
    {
        if (IsValid(ItemData))
        {
            if (ItemData->ItemID == ItemID)
            {
                // Store if ID match
                Result.Push(ItemData);
            }
        }
    }
    
    return Result;
}

const UMTD_MaterialItemData *UMTD_InventoryManagerComponent::FindIncompleteStackableItemEntryOfID(int32 ItemID) const
{
    // Get all stackable entries with given ID
    TArray<const UMTD_BaseInventoryItemData *> MaterialInstances = FindItemsOfID(ItemID);

    // Iterate through each entry, and return the first whose max capacity is not reached
    for (const auto Item : MaterialInstances)
    {
        const auto MaterialItemData = CastChecked<UMTD_MaterialItemData>(Item);
        if (MaterialItemData->MaxAmount > MaterialItemData->CurrentAmount)
        {
            return MaterialItemData;
        }
    }

    return nullptr;
}

UMTD_MaterialItemData *UMTD_InventoryManagerComponent::FindIncompleteStackableItemEntryOfID(int32 ItemID)
{
    // Cast ownself to const, call const method version, cast consteness away from the result
    return const_cast<UMTD_MaterialItemData *>(
        (static_cast<const UMTD_InventoryManagerComponent *>(this))->FindIncompleteStackableItemEntryOfID(ItemID));
}

const UMTD_BaseInventoryItemData *UMTD_InventoryManagerComponent::FindItemOfID(int32 ItemID) const
{
    // Iterate through each entry and compare the item ID
    for (const UMTD_BaseInventoryItemData *ItemData : InventoryEntries)
    {
        if (IsValid(ItemData))
        {
            if (ItemData->ItemID == ItemID)
            {
                // Return if ID match
                return ItemData;
            }
        }
    }
    
    return nullptr;
}

UMTD_BaseInventoryItemData *UMTD_InventoryManagerComponent::FindItemOfID(int32 ItemID)
{
    // Cast ownself to const, call const method version, cast consteness away from the result
    return const_cast<UMTD_BaseInventoryItemData*>(
        (static_cast<const UMTD_InventoryManagerComponent*>(this))->FindItemOfID(ItemID));
}

bool UMTD_InventoryManagerComponent::ContainsItemID(int32 ItemID) const
{
    const UMTD_BaseInventoryItemData *ItemData = FindItemOfID(ItemID);
    const bool bFound = IsValid(ItemData);
    return bFound;
}

bool UMTD_InventoryManagerComponent::CanStackItem(int32 ItemID) const
{
    const UMTD_BaseInventoryItemData *StackableItemEntry = FindIncompleteStackableItemEntryOfID(ItemID);
    const bool bCanStack = IsValid(StackableItemEntry);
    return bCanStack;
}

AMTD_BaseCharacter *UMTD_InventoryManagerComponent::GetCharacter() const
{
    const AActor *Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return nullptr;
    }

    const auto MtdPlayerState = CastChecked<AMTD_PlayerState>(Owner);
    AMTD_BasePlayerCharacter *MtdCharacter = MtdPlayerState->GetMtdPlayerCharacter();

    return MtdCharacter;
}

void UMTD_InventoryManagerComponent::AddWeaponItem(UMTD_BaseInventoryItemData *ItemData)
{
    check(IsValid(ItemData));
    InventoryEntries.Push(ItemData);
}

void UMTD_InventoryManagerComponent::AddArmorItem(UMTD_BaseInventoryItemData *ItemData)
{
    check(IsValid(ItemData));
    InventoryEntries.Push(ItemData);
}

void UMTD_InventoryManagerComponent::AddMaterialItem(UMTD_BaseInventoryItemData *ItemData)
{
    check(IsValid(ItemData));
    
    UMTD_MaterialItemData *StackableItemEntry = FindIncompleteStackableItemEntryOfID(ItemData->ItemID);
    if (!IsValid(StackableItemEntry))
    {
        // Make a new entry
        InventoryEntries.Push(ItemData);

        // Use the passed item data as the stackable item entry
        StackableItemEntry = Cast<UMTD_MaterialItemData>(ItemData);
    }
    
    // Increase the counter
    StackableItemEntry->CurrentAmount++;
}
