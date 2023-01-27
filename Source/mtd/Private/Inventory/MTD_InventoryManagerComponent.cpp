#include "Inventory/MTD_InventoryManagerComponent.h"

#include "Character/MTD_BasePlayerCharacter.h"
#include "Inventory/MTD_InventoryItemInstance.h"
#include "Inventory/Items/MTD_BaseInventoryItemData.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Inventory/Items/MTD_InventoryItemsCoreTypes.h"
#include "Inventory/Items/MTD_MaterialItemData.h"
#include "Player/MTD_PlayerState.h"

UMTD_InventoryManagerComponent::UMTD_InventoryManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UMTD_InventoryManagerComponent::CanAddItem(const UMTD_BaseInventoryItemData *ItemData) const
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Base Inventory Item Data is invalid.");
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

bool UMTD_InventoryManagerComponent::AddItem(UMTD_BaseInventoryItemData *ItemData,
    bool bDropIfNotEnoughSpace /* = false */)
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Inventory Item Data is invalid.");
        return false;
    }

    if (!CanAddItem(ItemData))
    {
        if (!bDropIfNotEnoughSpace)
        {
            MTDS_WARN("Item [%s] cannot be added to inventory.", *ItemData->GetName());
        }
        else
        {
            MTDS_WARN("Item [%s] cannot be added to inventory. It will dropped on the floor.", *ItemData->GetName());
            DropItem(ItemData);
        }
        return false;
    }
    
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
        return false;
    }

    // Notify about the event
    OnItemAddedDelegate.Broadcast(ItemData);
    
    MTDS_LOG("Item [%s] has been added to inventory list.", *ItemData->Name.ToString());
    return true;
}

bool UMTD_InventoryManagerComponent::RemoveItem(UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Inventory Item Data is invalid.");
        return false;;
    }

    const int32 RemovedAmount = InventoryEntries.Remove(ItemData);
    if (RemovedAmount == 0)
    {
        MTDS_WARN("Item [%s] is not contained inside the Inventory ManagerComponent.", *ItemData->GetName());
        return false;
    }

    // Notify about the event
    OnItemRemovedDelegate.Broadcast(ItemData);

    MTDS_LOG("Removed [%d] items from inventory.", RemovedAmount);
    return true;
}

AMTD_InventoryItemInstance *UMTD_InventoryManagerComponent::DropItem(UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item Data is invalid.");
        return nullptr;
    }
    
    const AActor *Owner = GetOwner();
    if (!IsValid(Owner))
    {
        MTDS_WARN("Owner is invalid.");
        return nullptr;
    }

    const auto MtdPs = Cast<AMTD_PlayerState>(Owner);
    check(MtdPs);

    AMTD_BasePlayerCharacter *MtdCharacter = MtdPs->GetMtdPlayerCharacter();
    AMTD_InventoryItemInstance *ItemInstance = UMTD_InventoryBlueprintFunctionLibrary::CreateItemInstance(
        MtdCharacter, ItemData);

    if (!IsValid(ItemInstance))
    {
        MTDS_WARN("Failed to spawn Item Instance on Character [%s].", *GetNameSafe(MtdCharacter));
        return nullptr;
    }

    MTDS_LOG("Item [%s] has been dropped [%s].", *ItemData->GetName(), *ItemInstance->GetName());
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

TArray<const UMTD_BaseInventoryItemData*> UMTD_InventoryManagerComponent::GetAllInstancesItemID(int32 ItemID) const
{
    TArray<const UMTD_BaseInventoryItemData*> Result;
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

TArray<UMTD_BaseInventoryItemData*> UMTD_InventoryManagerComponent::GetAllInstancesItemID(int32 ItemID)
{
    const auto ThisConst = static_cast<const UMTD_InventoryManagerComponent*>(this);
    TArray<const UMTD_BaseInventoryItemData*> ConstInstances = ThisConst->GetAllInstancesItemID(ItemID);
    TArray<UMTD_BaseInventoryItemData*> Instances;

    for (const UMTD_BaseInventoryItemData *ConstItemData : ConstInstances)
    {
        const auto ItemData = const_cast<UMTD_BaseInventoryItemData*>(ConstItemData);
        Instances.Push(ItemData);
    }

    return Instances;
}

const UMTD_MaterialItemData *UMTD_InventoryManagerComponent::GetStackableItemEntry(int32 ItemID) const
{
    TArray<const UMTD_BaseInventoryItemData*> MaterialInstances = GetAllInstancesItemID(ItemID);
    for (const auto It : MaterialInstances)
    {
        const auto MaterialItemData = Cast<UMTD_MaterialItemData>(It);
        check(MaterialItemData);

        if (MaterialItemData->MaxAmount > MaterialItemData->CurrentAmount)
        {
            return MaterialItemData;
        }
    }

    return nullptr;
}

UMTD_MaterialItemData *UMTD_InventoryManagerComponent::GetStackableItemEntry(int32 ItemID)
{
    return const_cast<UMTD_MaterialItemData*>(
        (static_cast<const UMTD_InventoryManagerComponent*>(this))->GetStackableItemEntry(ItemID));
}

const UMTD_BaseInventoryItemData *UMTD_InventoryManagerComponent::FindItemID(int32 ItemID) const
{
    for (const UMTD_BaseInventoryItemData *ItemData : InventoryEntries)
    {
        if (IsValid(ItemData))
        {
            if (ItemData->ItemID == ItemID)
            {
                return ItemData;
            }
        }
    }
    
    return nullptr;
}

UMTD_BaseInventoryItemData *UMTD_InventoryManagerComponent::FindItemID(int32 ItemID)
{
    return const_cast<UMTD_BaseInventoryItemData*>(
        (static_cast<const UMTD_InventoryManagerComponent*>(this))->FindItemID(ItemID));
}

bool UMTD_InventoryManagerComponent::ContainsItemID(int32 ItemID) const
{
    const UMTD_BaseInventoryItemData *ItemData = FindItemID(ItemID);
    return IsValid(ItemData);
}

bool UMTD_InventoryManagerComponent::CanStackItem(int32 ItemID) const
{
    const UMTD_BaseInventoryItemData *StackableItemEntry = GetStackableItemEntry(ItemID);
    return IsValid(StackableItemEntry);
}

void UMTD_InventoryManagerComponent::AddWeaponItem(UMTD_BaseInventoryItemData *ItemData)
{
    check(ItemData);
    InventoryEntries.Push(ItemData);
}

void UMTD_InventoryManagerComponent::AddArmorItem(UMTD_BaseInventoryItemData *ItemData)
{
    check(ItemData);
    InventoryEntries.Push(ItemData);
}

void UMTD_InventoryManagerComponent::AddMaterialItem(UMTD_BaseInventoryItemData *ItemData)
{
    check(ItemData);
    UMTD_MaterialItemData *StackableItemEntry = GetStackableItemEntry(ItemData->ItemID);

    if (!IsValid(StackableItemEntry))
    {
        // Make a new entry
        InventoryEntries.Push(ItemData);

        // Use the passed item data as the stackable item entry
        StackableItemEntry = Cast<UMTD_MaterialItemData>(ItemData);
    }
    
    // Increase the count
    StackableItemEntry->CurrentAmount++;
}
