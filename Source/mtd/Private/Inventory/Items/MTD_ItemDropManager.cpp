#include "Inventory/Items/MTD_ItemDropManager.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BaseEnemyCharacter.h"

UMTD_ItemDropManager::UMTD_ItemDropManager()
{
    ensure(DropChance > 0.f);
    ensure(DropChance <= 100.f);
    DropChance = FMath::Clamp(DropChance, 0.f, 100.f);
}

void UMTD_ItemDropManager::Init()
{
    ClassifiedUnsortedItemDropListSet ClassifiedUnsortedSet;
    
    auto AddItemDropLists = [&, this] (const UnsortedItemDropListSet &ItemDropListsPool, const CharacterClass &Class)
        {
            for (const ItemPtr &ItemDropsList : ItemDropListsPool)
            {
                // Add the set only if the set has "All" hero class
                if (FMTD_GameplayTags::IsForAllHeroClasses(ItemDropsList->HeroClasses))
                {
                    UnsortedItemDropListSet &UnsortedSet = ClassifiedUnsortedSet.FindOrAdd(Class);
                    UnsortedSet.Add(ItemDropsList);
                }
            }
        };
    
    // Dispatch targetted lists 
    for (const auto &[Class, ItemDropsListSet] : ClassifiedItemDropListPool)
    {
        AddItemDropLists(ItemDropsListSet.ItemDropsData, Class);
    }

    // Dispatch broadcast lists
    for (const TSubclassOf<AMTD_BaseEnemyCharacter> Class : BroadcastListeners)
    {
        AddItemDropLists(BroadcastItemDropListPool, Class);
    }

    // Add gathered lists to the global one
    AddListsInGlobalPool(ClassifiedUnsortedSet);
    MTDS_LOG("Item Drop Manager has been initialized.");
}

UMTD_ItemDropManager *UMTD_ItemDropManager::Construct()
{
    if (IsValid(ItemDropManager))
    {
        MTD_WARN("Item Drop Manager is already instantiated.");
        return nullptr;
    }
    
    // Get the class name defined in config
    FString ClassName;
    GConfig->GetString(TEXT("ItemDropManager"), TEXT("DefaultItemDropManagerClass"), ClassName, GGameIni);

    // Get the actual class pointer
    const UClass *DefaultClass = LoadClass<UMTD_ItemDropManager>(nullptr, *ClassName);
    if (!DefaultClass)
    {
        MTD_WARN("Default Item Drop Manager Class is NULL.");
        return nullptr;
    }
    
    // Create a new instance, and disallow default garbage collecting
    constexpr EObjectFlags Flags = (EObjectFlags::RF_Transient | EObjectFlags::RF_MarkAsRootSet);
    ItemDropManager = NewObject<UMTD_ItemDropManager>(GetTransientPackage(), DefaultClass, NAME_None, Flags);
    ItemDropManager->Init();

    if (!IsValid(ItemDropManager))
    {
        MTD_WARN("Failed to create Item Drop Manager.");
        return nullptr;
    }

    MTD_LOG("Item Drop Manager has been constructed.");
    return ItemDropManager;
}

void UMTD_ItemDropManager::Destroy()
{
    if (IsValid(ItemDropManager))
    {
        ItemDropManager->RemoveFromRoot();
        ItemDropManager->MarkAsGarbage();
        
        MTD_LOG("Item Drop Manager has been destroyed.");
    }
}

UMTD_ItemDropManager *UMTD_ItemDropManager::Get()
{
    return ItemDropManager;
}

void UMTD_ItemDropManager::AddListInPoolBroadcast(const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    if (!IsValid(ItemDropList))
    {
        MTDS_WARN("Item Drops Data is invalid.");
        return;
    }

    // Avoid adding already contained items
    if (BroadcastItemDropListPool.Contains(ItemDropList))
    {
        MTDS_WARN("Item Drop Data [%s] is already contained in Broadcast Item Drop List Pool.", 
            *ItemDropList->GetName());
        return;
    }

    // Add item drop list to broadcast pool
    BroadcastItemDropListPool.Add(ItemDropList);
    MTDS_LOG("Item Drop List [%s] has been added to list broadcast pool.", *ItemDropList->GetName());

    // Add new list to global list only if there is at least one hero class that satisfies the given list
    if (ItemDropList->HeroClasses.HasAny(ActiveHeroClasses))
    {
        ClassifiedUnsortedItemDropListSet ClassifiedUnsortedSet;
        
        // Gather all listeners into a single container
        for (const TSubclassOf<AMTD_BaseEnemyCharacter> &Class : BroadcastListeners)
        {
            UnsortedItemDropListSet &GlobalUnsortedSet = ClassifiedUnsortedSet.Add(Class);
            GlobalUnsortedSet.Add(ItemDropList);
        }

        // Make a single call
        AddListsInGlobalPool(ClassifiedUnsortedSet, bSort);
    }
}

void UMTD_ItemDropManager::AddListInPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    if (!IsValid(Class))
    {
        MTDS_WARN("Class is invalid.");
        return;
    }
    
    if (!IsValid(ItemDropList))
    {
        MTDS_WARN("Item Drops Data is invalid.");
        return;
    }

    // Find entry for given class
    FMTD_ItemDropListSet &ClassifiedUnsortedSet = ClassifiedItemDropListPool.FindOrAdd(Class);
    
    // Avoid adding already contained items
    if (ClassifiedUnsortedSet.ItemDropsData.Contains(ItemDropList))
    {
        MTDS_WARN("Item Drop Data [%s] is already contained in Item Drop List Pool for Class [%s].", 
            *ItemDropList->GetName(), *Class->GetName());
        return;
    }

    // Add item drop list to classified pool
    ClassifiedUnsortedSet.ItemDropsData.Add(ItemDropList);
    MTDS_LOG("Item Drop List [%s] has been added to list pool for Class [%s].", *ItemDropList->GetName(), 
        *Class->GetName());

    // Add new list to global list only if there is at least one hero class that satisfies the given list
    if (ItemDropList->HeroClasses.HasAny(ActiveHeroClasses))
    {
        AddListInGlobalPool(Class, ItemDropList, bSort);
    }
}

void UMTD_ItemDropManager::AddClassifiedListsInPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists,
    bool bSort)
{
    bool bChanged = false;
    
    // Dispatch given classified lists
    for (const auto &[Class, ClassItemDropLists] : InItemDropLists)
    {
        for (const auto &ItemDropList : ClassItemDropLists)
        {
            // Add a new list, avoid sorting each time
            AddListInPool(Class, ItemDropList, /* bSort */ false);
            bChanged = true;
        }
    }

    // Sort only if a new set has been added
    if (((bSort) && (bChanged)))
    {
        // Keep the lists sorted
        SortGlobalItemDropLists();
    }
}

void UMTD_ItemDropManager::RemoveListInPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    if (!IsValid(Class))
    {
        MTDS_WARN("Class is invalid.");
        return;
    }
    
    if (!IsValid(ItemDropList))
    {
        MTDS_WARN("Item Drops Data is invalid.");
        return;
    }

    // Find classified set for this class
    FMTD_ItemDropListSet *ClassifiedUnsortedSet = ClassifiedItemDropListPool.Find(Class);
    if (!ClassifiedUnsortedSet)
    {
        MTDS_WARN("Class [%s] is not present in Classified Item Drop List Pool.", *Class->GetName());
        return;
    }
    
    // Try to remove given list
    const int32 NumRemovedItems = ClassifiedUnsortedSet->ItemDropsData.Remove(ItemDropList);
    if (NumRemovedItems == 0)
    {
        MTDS_WARN("Item Drop List [%s] is not present in Classified Item Drop List Pool for Class [%s].",
            *ItemDropList->GetName(), *Class->GetName());
        return;
    }

    RemoveListFromGlobalPool(Class, ItemDropList, bSort);
}

void UMTD_ItemDropManager::RemoveClassifiedListsFromPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists,
    bool bSort)
{
    bool bChanged = false;
    
    // Dispatch given classified lists
    for (const auto &[Class, ClassItemDropLists] : InItemDropLists)
    {
        for (const auto &ItemDropList : ClassItemDropLists)
        {
            // Remove a list, avoid sorting each time
            RemoveListInPool(Class, ItemDropList, /* bSort */ false);
            bChanged = true;
        }
    }

    // Sort only if a set has been removed
    if (((bSort) && (bChanged)))
    {
        // Keep the lists sorted
        SortGlobalItemDropLists();
    }
}

void UMTD_ItemDropManager::AddHeroClass(const FGameplayTag &HeroClass)
{
    AddHeroClasses(FGameplayTagContainer(HeroClass));
}

void UMTD_ItemDropManager::AddHeroClasses(const FGameplayTagContainer &HeroClasses)
{
    FGameplayTagContainer NewHeroClasses;

    // Add only the ones currently absent, and save them
    for (const FGameplayTag &HeroClass : HeroClasses)
    {
        if (!ActiveHeroClasses.HasTag(HeroClass))
        {
            ActiveHeroClasses.AddTag(HeroClass);
            NewHeroClasses.AddTag(HeroClass);
            
            MTDS_LOG("New Hero Class has been added: [%s].", *HeroClass.ToString());
        }
    }

    if (!NewHeroClasses.IsEmpty())
    {
        // Try to add new lists for new classes
        AddListsToGlobalDropsForClasses(NewHeroClasses);
    }
}

void UMTD_ItemDropManager::RemoveHeroClass(const FGameplayTag &HeroClass)
{
    RemoveHeroClasses(FGameplayTagContainer(HeroClass));
}

void UMTD_ItemDropManager::RemoveHeroClasses(const FGameplayTagContainer &HeroClasses)
{
    FGameplayTagContainer RemovedHeroClasses;
    for (const FGameplayTag &HeroClass : HeroClasses)
    {
        if (!ActiveHeroClasses.HasTag(HeroClass))
        {
            ActiveHeroClasses.RemoveTag(HeroClass);
            RemovedHeroClasses.AddTag(HeroClass);
        }
    }

    if (!RemovedHeroClasses.IsEmpty())
    {
        // Try to remove lists for removed classes
        RemoveListsFromGlobalDropsForClasses(RemovedHeroClasses);
    }
}

void UMTD_ItemDropManager::AddListInGlobalPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    UnsortedItemDropListSet UnsortedListSet { ItemDropList };
    ClassifiedUnsortedItemDropListSet ClassifiedUnsortedListSet { { Class, UnsortedListSet } };
    AddListsInGlobalPool(ClassifiedUnsortedListSet, bSort);
}

void UMTD_ItemDropManager::AddListsInGlobalPool(ClassifiedUnsortedItemDropListSet &ClassifiedUnsortedSet, bool bSort)
{
    bool bChanged = false;
    
    // Dispatch all item drop lists
    for (const auto &[Class, ClassUnsortedSet] : ClassifiedUnsortedSet)
    {
        // Get unsorted item drop list set for current class
        UnsortedItemDropListSet &GlobalClassUnsortedSet = GlobalUnsortedItemDropList.FindOrAdd(Class);

        // Add each item from the given set for current class in global pool
        for (const UMTD_ItemDropList *ItemDropList : ClassUnsortedSet)
        {
            if (!GlobalClassUnsortedSet.Contains(ItemDropList))
            {
                GlobalClassUnsortedSet.Add(ItemDropList);
                bChanged = true;
            }
        }

        const int32 NumItems = ClassUnsortedSet.Num();
        if (NumItems > 0)
        {
            MTDS_LOG("[%d] Item Drop Lists have added to Global Item Pool for Class [%s].",
                NumItems, *Class->GetName());
        }
    }

    // Sort only if a set has been removed
    if (((bSort) && (bChanged)))
    {
        // Keep the lists sorted
        SortGlobalItemDropLists();
    }
}

void UMTD_ItemDropManager::RemoveListFromGlobalPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    UnsortedItemDropListSet UnsortedListSet { ItemDropList };
    ClassifiedUnsortedItemDropListSet ClassifiedUnsortedListSet { { Class, UnsortedListSet } };
    RemoveListsFromGlobalPool(ClassifiedUnsortedListSet, bSort);
}

void UMTD_ItemDropManager::RemoveListsFromGlobalPool(ClassifiedUnsortedItemDropListSet &RemovedItems, bool bSort)
{
    // Dispatch all item drop lists
    for (const auto &[Class, ClassUnsortedSet] : RemovedItems)
    {
        // Find classified set for this class in global pool
        UnsortedItemDropListSet *GlobalUnsortedSet = GlobalUnsortedItemDropList.Find(Class);
        if (!GlobalUnsortedSet)
        {
            MTDS_WARN("Class [%s] is not present in Global Unsorted Item Drop List Pool.", *Class->GetName());
            continue;
        }

        // Iterate through each item drop list
        for (const auto &ItemDropList : ClassUnsortedSet)
        {
            // Try to remove given list from global pool
            const int32 NumRemovedItems = GlobalUnsortedSet->Remove(ItemDropList);
            if (NumRemovedItems == 0)
            {
                MTDS_WARN("Item Drop List [%s] is not present in Global Unsorted Item Drop List Pool for Class [%s].",
                    *ItemDropList->GetName(), *Class->GetName());
            }
        }
        
        const int32 NumItems = ClassUnsortedSet.Num();
        if (NumItems > 0)
        {
            MTDS_LOG("[%d] Item Drop Lists have removed from Global Item Pool for Class [%s].",
                NumItems, *Class->GetName());
        }
    }

    if (bSort)
    {
        // Keep the lists sorted
        SortGlobalItemDropLists();
    }
}

void UMTD_ItemDropManager::AddListsToGlobalDropsForClasses(const FGameplayTagContainer &AddedHeroClasses)
{
    ClassifiedUnsortedItemDropListSet ClassifiedSet;
    
    auto AddItemDropLists = [&, this] (const UnsortedItemDropListSet &ItemDropListsPool, const CharacterClass &Class)
        {
            // Create a new entry to store the item drop lists in
            UnsortedItemDropListSet &ClassLocalUnsortedSet = ClassifiedSet.Add(Class);
        
            // Get item drop lists for current class to avoid adding the same ones
            const UnsortedItemDropListSet *FoundGlobalClassUnsortedSet = GlobalUnsortedItemDropList.Find(Class);
        
            for (const ItemPtr &ItemDropsList : ItemDropListsPool)
            {
                // Add the set only if any of its hero classes match with new ones
                if (ItemDropsList->HeroClasses.HasAny(AddedHeroClasses))
                {
                    // Check whether the set is already added. A set may be have several hero classes
                    if (((!FoundGlobalClassUnsortedSet) || (!FoundGlobalClassUnsortedSet->Contains(ItemDropsList))))
                    {
                        ClassLocalUnsortedSet.Add(ItemDropsList);
                    }
                }
            }
        };
    
    // Dispatch targetted lists 
    for (const auto &[Class, ItemDropsListSet] : ClassifiedItemDropListPool)
    {
        AddItemDropLists(ItemDropsListSet.ItemDropsData, Class);
    }

    // Dispatch broadcast lists
    for (const TSubclassOf<AMTD_BaseEnemyCharacter> Class : BroadcastListeners)
    {
        AddItemDropLists(BroadcastItemDropListPool, Class);
    }

    AddClassifiedListsInPool(ClassifiedSet);
}

void UMTD_ItemDropManager::RemoveListsFromGlobalDropsForClasses(const FGameplayTagContainer &RemovedHeroClasses)
{
    ClassifiedUnsortedItemDropListSet ClassifiedSet;

    // Iterate through all added item drop lists
    for (const auto &[Class, ItemDropsListSet] : GlobalUnsortedItemDropList)
    {
        // Get reference to current class item drop lists
        for (const UMTD_ItemDropList *ItemDropsList : ItemDropsListSet)
        {
            // Avoid deleting items that are supposed to be dropped regardless of active hero classes
            if (FMTD_GameplayTags::IsForAllHeroClasses(ItemDropsList->HeroClasses))
            {
                continue;
            }
            
            // Delete current item drop list if any its hero classes match with the remained ones
            if (ItemDropsList->HeroClasses.HasAny(ActiveHeroClasses))
            {
                UnsortedItemDropListSet &ClassSet = ClassifiedSet.FindOrAdd(Class);
                ClassSet.Add(ItemDropsList);
            }
        }
    }

    RemoveClassifiedListsFromPool(ClassifiedSet);
}

void UMTD_ItemDropManager::SortGlobalItemDropLists()
{
    // Sort gathered data
    for (const auto &[Class, ClassItemDropLists] : GlobalUnsortedItemDropList)
    {
        // Find max weight
        float MaxWeight = 0.f;
        for (const UMTD_ItemDropList *ItemDropList : ClassItemDropLists)
        {
            // Every weight should be a non-negative number
            ensure(ItemDropList->Weight > 0.f);

            // Select max weight
            if (MaxWeight < ItemDropList->Weight)
            {
                MaxWeight = ItemDropList->Weight;
            }
        }

        // Create sorted item drop list container with normalized weights as keys
        SortedItemDropListSet ClassSortedItemDropLists;
        for (const UMTD_ItemDropList *ItemDropList : ClassItemDropLists)
        {
            const float NormalizedWeight = (ItemDropList->Weight / MaxWeight);
            ClassSortedItemDropLists.Add(NormalizedWeight, ItemDropList);

            MTDS_VERBOSE("Class [%s] has received an Item Drop List [%s] with Normalized Weight [%f].",
                *Class->GetName(), *ItemDropList->GetName(), NormalizedWeight);
        }

        // Delete old values
        GlobalSortedItemDropList.Remove(Class);

        // Bind the computed data with character class
        GlobalSortedItemDropList.Add(Class, ClassSortedItemDropLists);
    }
}

bool UMTD_ItemDropManager::GetRandomItemDropList(TSubclassOf<AMTD_BaseEnemyCharacter> Class,
    const UMTD_ItemDropList **OutItemDropList) const
{
    if (!OutItemDropList)
    {
        MTDS_WARN("Pointer to pointer to Item Drop List is NULL.");
        return false;
    }
    
    *OutItemDropList = nullptr;
    
    if (!IsValid(Class))
    {
        MTDS_WARN("Class is invalid.");
        return false;
    }

    const float RandChance = FMath::FRandRange(0.f, 100.f);
    if (RandChance > DropChance)
    {
        // Randomized value is too low to give an item
        return false;
    }

    const SortedItemDropListSet *Container = GlobalSortedItemDropList.Find(Class);
    if (!Container)
    {
        // There are no items to drop for that character
        return false;
    }

    // Search for the lowest possible weight
    const float RandWeight = FMath::FRand();
    for (const auto &[Weight, List] : *Container)
    {
        if (Weight >= RandWeight)
        {
            *OutItemDropList = List;
            return true;
        }
    }

    // Should never happen
    ensureAlways(false);
    return false;
}

bool UMTD_ItemDropManager::GetRandomItemID(TSubclassOf<AMTD_BaseEnemyCharacter> Class, int32 &OutItemID) const
{
    if (!IsValid(Class))
    {
        MTDS_WARN("Class is invalid.");
        return false;
    }

    const UMTD_ItemDropList *ItemDropList = nullptr;
    if (!GetRandomItemDropList(Class, /* OutItemDropList */ &ItemDropList))
    {
        return false;
    }
    
    if (!IsValid(ItemDropList))
    {
        MTDS_WARN("Item Drop List is invalid.");
        return false;
    }

    // Get a random element from the randomized list
    const int32 Num = ItemDropList->ItemIDs.Num();
    const int32 RandNum = FMath::RandRange(0, (Num - 1));
    const int32 RandID = ItemDropList->ItemIDs[RandNum];
    
    OutItemID = RandID;
    return true;
}
