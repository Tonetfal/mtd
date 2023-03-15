#include "InventorySystem/MTD_ItemDropSubsystem.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BaseFoeCharacter.h"
#include "Settings/MTD_ItemDropSettings.h"

UMTD_ItemDropSubsystem *UMTD_ItemDropSubsystem::Get(const UObject *WorldContextObject)
{
    if (!IsValid(WorldContextObject))
    {
        MTD_WARN("World context object is invalid.");
        return nullptr;
    }
    
    const UWorld *World = WorldContextObject->GetWorld();
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return nullptr;
    }
    
    const auto Subsystem = World->GetSubsystem<UMTD_ItemDropSubsystem>();
    if (!IsValid(Subsystem))
    {
        MTD_WARN("Item drop subsystem is invalid.");
        return nullptr;
    }
    
    return Subsystem;
}

void UMTD_ItemDropSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    Super::Initialize(Collection);
    ReadSettings();
    
    ensure(DropChance > 0.f);
    ensure(DropChance <= 100.f);
    DropChance = FMath::Clamp(DropChance, 0.f, 100.f);
    
    ClassifiedUnsortedItemDropListSet ClassifiedUnsortedSet;
    
    auto AddItemDropLists = [&, this] (const UnsortedItemDropListSet &ItemDropListsPool, const CharacterClass &Class)
        {
            for (const ItemPtr &ItemDropsList : ItemDropListsPool)
            {
                const UMTD_ItemDropList *LoadedList = ItemDropsList.LoadSynchronous();
                
                // Add the set only if the set has "All" hero class
                if (IsValid(LoadedList) && FMTD_GameplayTags::IsForAllHeroClasses(ItemDropsList->HeroClasses))
                {
                    UnsortedItemDropListSet &UnsortedSet = ClassifiedUnsortedSet.FindOrAdd(Class);
                    UnsortedSet.Add(ItemDropsList);
                }
            }
        };
    
    // Dispatch targetted lists 
    for (const auto &[Class, ItemDropsListSet] : ClassifiedItemDropListPool)
    {
        AddItemDropLists(ItemDropsListSet.ItemDropsData, Class.LoadSynchronous());
    }

    // Dispatch broadcast lists
    for (const TSoftClassPtr<AMTD_BaseFoeCharacter> Class : BroadcastListeners)
    {
        AddItemDropLists(BroadcastItemDropListPool, Class.LoadSynchronous());
    }

    // Add gathered lists to the global one
    AddListsInActivePool(ClassifiedUnsortedSet);
    MTDS_LOG("Item drop manager has been initialized.");
}

void UMTD_ItemDropSubsystem::ReadSettings()
{
    const auto ItemDropSettings = UMTD_ItemDropSettings::Get();
    check(IsValid(ItemDropSettings));

    // It's supposed that they are empty by default
    ensure(ClassifiedItemDropListPool.IsEmpty());
    ensure(BroadcastItemDropListPool.IsEmpty());
    ensure(BroadcastListeners.IsEmpty());

    // Copy settings values
    DropChance = ItemDropSettings->DropChance;
    ClassifiedItemDropListPool = ItemDropSettings->ClassifiedItemDropListPool;
    BroadcastItemDropListPool = ItemDropSettings->BroadcastItemDropListPool;
    BroadcastListeners = ItemDropSettings->BroadcastListeners;
}

bool UMTD_ItemDropSubsystem::GetRandomItemDropList(TSoftClassPtr<AMTD_BaseFoeCharacter> Class,
    const UMTD_ItemDropList **OutItemDropList) const
{
    if (!OutItemDropList)
    {
        MTDS_WARN("Pointer to pointer to item drop list is NULL.");
        return false;
    }
    
    *OutItemDropList = nullptr;
    
    if (!IsValid(Class.LoadSynchronous()))
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

    const SortedItemDropListSet *Container = ActiveSortedItemDropList.Find(Class);
    if (!Container)
    {
        MTDS_WARN("There are no items to drop for class [%s].", *Class->GetName());
        return false;
    }

    // Randomize a weight value [0.0-1.0]
    const float RandWeight = FMath::FRand();
    
    // Search for the highest possible weight. Note that container is sorted in ascending order
    for (const auto &[ListWeight, List] : *Container)
    {
        if (!(*OutItemDropList))
        {
            // Give a default value
            *OutItemDropList = List.LoadSynchronous();
        }
        
        if (ListWeight <= RandWeight)
        {
            // List weight is less or equal to the randomized weight, hence save it for now
            *OutItemDropList = List.LoadSynchronous();
        }
        else
        {
            // Current list weights more than the randomized weight, hence the last saved item drop list has the
            // greatest possible value for the randomized weight
            break;
        }
    }

    if (*OutItemDropList)
    {
        // An item drop list has been found
        return true;
    }

    // No item drop list has been found. Should never happen
    ensure(false);
    return false;
}

bool UMTD_ItemDropSubsystem::GetRandomItemID(TSoftClassPtr<AMTD_BaseFoeCharacter> Class, int32 &OutItemID) const
{
    if (!IsValid(Class.LoadSynchronous()))
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
        MTDS_WARN("Item drop list is invalid.");
        return false;
    }

    // Get a random element from the randomized list
    const int32 Num = ItemDropList->ItemIDs.Num();
    const int32 RandNum = FMath::RandRange(0, (Num - 1));
    const int32 RandID = ItemDropList->ItemIDs[RandNum];
    
    OutItemID = RandID;
    return true;
}

void UMTD_ItemDropSubsystem::AddListInPoolBroadcast(const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    if (!IsValid(ItemDropList))
    {
        MTDS_WARN("Item drops data is invalid.");
        return;
    }

    // Avoid adding already contained items
    if (BroadcastItemDropListPool.Contains(ItemDropList))
    {
        MTDS_WARN("Item drop data [%s] is already contained in broadcast item drop list pool.", 
            *ItemDropList->GetName());
        return;
    }

    // Add item drop list to broadcast pool
    BroadcastItemDropListPool.Add(ItemDropList);
    MTDS_LOG("Item drop list [%s] has been added to list broadcast pool.", *ItemDropList->GetName());

    // Add new list to global list only if there is at least one hero class that satisfies the given list
    if (ItemDropList->HeroClasses.HasAny(ActiveHeroClasses))
    {
        ClassifiedUnsortedItemDropListSet ClassifiedUnsortedSet;
        
        // Gather all listeners into a single container
        for (const TSoftClassPtr<AMTD_BaseFoeCharacter> &Class : BroadcastListeners)
        {
            UnsortedItemDropListSet &ActiveUnsortedSet = ClassifiedUnsortedSet.Add(Class);
            ActiveUnsortedSet.Add(ItemDropList);
        }

        // Make a single call
        AddListsInActivePool(ClassifiedUnsortedSet, bSort);
    }
}

void UMTD_ItemDropSubsystem::AddListInPool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    if (!IsValid(Class.LoadSynchronous()))
    {
        MTDS_WARN("Class is invalid.");
        return;
    }
    
    if (!IsValid(ItemDropList))
    {
        MTDS_WARN("Item drops data is invalid.");
        return;
    }

    // Find entry for given class
    FMTD_ItemDropListSet &ClassifiedUnsortedSet = ClassifiedItemDropListPool.FindOrAdd(Class);
    
    // Avoid adding already contained items
    if (ClassifiedUnsortedSet.ItemDropsData.Contains(ItemDropList))
    {
        MTDS_WARN("Item drop data [%s] is already contained in item drop list pool for class [%s].", 
            *ItemDropList->GetName(), *Class->GetName());
        return;
    }

    // Add item drop list to classified pool
    ClassifiedUnsortedSet.ItemDropsData.Add(ItemDropList);
    MTDS_LOG("Item drop list [%s] has been added to list pool for class [%s].", *ItemDropList->GetName(), 
        *Class->GetName());

    // Add new list to global list only if there is at least one hero class that satisfies the given list
    if (ItemDropList->HeroClasses.HasAny(ActiveHeroClasses))
    {
        AddListInActivePool(Class, ItemDropList, bSort);
    }
}

void UMTD_ItemDropSubsystem::AddClassifiedListsInPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists,
    bool bSort)
{
    bool bChanged = false;
    
    // Dispatch given classified lists
    for (const auto &[Class, ClassItemDropLists] : InItemDropLists)
    {
        for (const auto &ItemDropList : ClassItemDropLists)
        {
            // Add a new list, avoid sorting each time
            AddListInPool(Class, ItemDropList.LoadSynchronous(), /* bSort */ false);
            bChanged = true;
        }
    }

    // Sort only if a new set has been added
    if (((bSort) && (bChanged)))
    {
        // Keep the lists sorted
        SortActiveItemDropListsPool();
    }
}

void UMTD_ItemDropSubsystem::RemoveListInPool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    if (!IsValid(Class.LoadSynchronous()))
    {
        MTDS_WARN("Class is invalid.");
        return;
    }
    
    if (!IsValid(ItemDropList))
    {
        MTDS_WARN("Item drops data is invalid.");
        return;
    }

    // Find classified set for this class
    FMTD_ItemDropListSet *ClassifiedUnsortedSet = ClassifiedItemDropListPool.Find(Class);
    if (!ClassifiedUnsortedSet)
    {
        MTDS_WARN("Class [%s] is not present in classified item drop list pool.", *Class->GetName());
        return;
    }
    
    // Try to remove given list
    const int32 NumRemovedItems = ClassifiedUnsortedSet->ItemDropsData.Remove(ItemDropList);
    if (NumRemovedItems == 0)
    {
        MTDS_WARN("Item drop list [%s] is not present in classified item drop list pool for class [%s].",
            *ItemDropList->GetName(), *Class->GetName());
        return;
    }

    RemoveListFromActivePool(Class, ItemDropList, bSort);
}

void UMTD_ItemDropSubsystem::RemoveClassifiedListsFromPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists,
    bool bSort)
{
    bool bChanged = false;
    
    // Dispatch given classified lists
    for (const auto &[Class, ClassItemDropLists] : InItemDropLists)
    {
        for (const auto &ItemDropList : ClassItemDropLists)
        {
            // Remove a list, avoid sorting each time
            RemoveListInPool(Class, ItemDropList.LoadSynchronous(), /* bSort */ false);
            bChanged = true;
        }
    }

    // Sort only if a set has been removed
    if (((bSort) && (bChanged)))
    {
        // Keep the lists sorted
        SortActiveItemDropListsPool();
    }
}

void UMTD_ItemDropSubsystem::AddHeroClass(const FGameplayTag &HeroClass)
{
    AddHeroClasses(FGameplayTagContainer(HeroClass));
}

void UMTD_ItemDropSubsystem::AddHeroClasses(const FGameplayTagContainer &HeroClasses)
{
    FGameplayTagContainer NewHeroClasses;

    // Add only the ones currently absent, and save them
    for (const FGameplayTag &HeroClass : HeroClasses)
    {
        if (!ActiveHeroClasses.HasTag(HeroClass))
        {
            ActiveHeroClasses.AddTag(HeroClass);
            NewHeroClasses.AddTag(HeroClass);
            
            MTDS_LOG("New hero class [%s] has been added.", *HeroClass.ToString());
        }
    }

    if (!NewHeroClasses.IsEmpty())
    {
        // Try to add new lists for new classes
        AddListsToActivePoolForClasses(NewHeroClasses);
    }
}

void UMTD_ItemDropSubsystem::RemoveHeroClass(const FGameplayTag &HeroClass)
{
    RemoveHeroClasses(FGameplayTagContainer(HeroClass));
}

void UMTD_ItemDropSubsystem::RemoveHeroClasses(const FGameplayTagContainer &HeroClasses)
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
        RemoveListsFromActivePoolForClasses(RemovedHeroClasses);
    }
}

void UMTD_ItemDropSubsystem::AddListInActivePool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    UnsortedItemDropListSet UnsortedListSet { ItemDropList };
    ClassifiedUnsortedItemDropListSet ClassifiedUnsortedListSet { { Class.LoadSynchronous(), UnsortedListSet } };
    AddListsInActivePool(ClassifiedUnsortedListSet, bSort);
}

void UMTD_ItemDropSubsystem::AddListsInActivePool(ClassifiedUnsortedItemDropListSet &InDropItemLists, bool bSort)
{
    bool bChanged = false;
    
    // Dispatch all item drop lists
    for (const auto &[Class, ClassUnsortedSet] : InDropItemLists)
    {
        // Get unsorted item drop list set for current class
        UnsortedItemDropListSet &ActiveClassUnsortedSet = ActiveUnsortedItemDropList.FindOrAdd(Class);

        // Add each item from the given set for current class in global pool
        for (const TSoftObjectPtr<const UMTD_ItemDropList> ItemDropList : ClassUnsortedSet)
        {
            if (!ActiveClassUnsortedSet.Contains(ItemDropList))
            {
                ActiveClassUnsortedSet.Add(ItemDropList);
                bChanged = true;
            }
        }

        const int32 NumItems = ClassUnsortedSet.Num();
        if (NumItems > 0)
        {
            MTDS_LOG("Item drop lists [%d] have added to global item pool for class [%s].",
                NumItems, *Class->GetName());
        }
    }

    // Sort only if a set has been removed
    if (((bSort) && (bChanged)))
    {
        // Keep the lists sorted
        SortActiveItemDropListsPool();
    }
}

void UMTD_ItemDropSubsystem::RemoveListFromActivePool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class,
    const UMTD_ItemDropList *ItemDropList, bool bSort)
{
    UnsortedItemDropListSet UnsortedListSet { ItemDropList };
    ClassifiedUnsortedItemDropListSet ClassifiedUnsortedListSet { { Class.LoadSynchronous(), UnsortedListSet } };
    RemoveListsFromActivePool(ClassifiedUnsortedListSet, bSort);
}

void UMTD_ItemDropSubsystem::RemoveListsFromActivePool(ClassifiedUnsortedItemDropListSet &InDropItemLists, bool bSort)
{
    // Dispatch all item drop lists
    for (const auto &[Class, ClassUnsortedSet] : InDropItemLists)
    {
        // Find classified set for this class in global pool
        UnsortedItemDropListSet *ActiveUnsortedSet = ActiveUnsortedItemDropList.Find(Class);
        if (!ActiveUnsortedSet)
        {
            MTDS_WARN("Class [%s] is not present in global unsorted item drop list pool.", *Class->GetName());
            continue;
        }

        // Iterate through each item drop list
        for (const auto &ItemDropList : ClassUnsortedSet)
        {
            // Try to remove given list from global pool
            const int32 NumRemovedItems = ActiveUnsortedSet->Remove(ItemDropList);
            if (NumRemovedItems == 0)
            {
                MTDS_WARN("Item drop list [%s] is not present in global unsorted item drop list pool for class [%s].",
                    *ItemDropList->GetName(), *Class->GetName());
            }
        }
        
        const int32 NumItems = ClassUnsortedSet.Num();
        if (NumItems > 0)
        {
            MTDS_LOG("[%d] item drop lists have removed from global item pool for class [%s].",
                NumItems, *Class->GetName());
        }
    }

    if (bSort)
    {
        // Keep the lists sorted
        SortActiveItemDropListsPool();
    }
}

void UMTD_ItemDropSubsystem::AddListsToActivePoolForClasses(const FGameplayTagContainer &AddedHeroClasses)
{
    ClassifiedUnsortedItemDropListSet ClassifiedSet;
    
    auto AddItemDropLists = [&, this] (const UnsortedItemDropListSet &ItemDropListsPool, const CharacterClass &Class)
        {
            // Create a new entry to store the item drop lists in
            UnsortedItemDropListSet &ClassLocalUnsortedSet = ClassifiedSet.Add(Class);
        
            // Get item drop lists for current class to avoid adding the same ones
            const UnsortedItemDropListSet *FoundActiveClassUnsortedSet = ActiveUnsortedItemDropList.Find(Class);
        
            for (const ItemPtr &ItemDropsList : ItemDropListsPool)
            {
                // Add the set only if any of its hero classes match with new ones
                if (ItemDropsList->HeroClasses.HasAny(AddedHeroClasses))
                {
                    // Check whether the set is already added. A set may be have several hero classes
                    if (((!FoundActiveClassUnsortedSet) || (!FoundActiveClassUnsortedSet->Contains(ItemDropsList))))
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
    for (const TSoftClassPtr<AMTD_BaseFoeCharacter> Class : BroadcastListeners)
    {
        AddItemDropLists(BroadcastItemDropListPool, Class);
    }

    AddClassifiedListsInPool(ClassifiedSet);
}

void UMTD_ItemDropSubsystem::RemoveListsFromActivePoolForClasses(const FGameplayTagContainer &RemovedHeroClasses)
{
    ClassifiedUnsortedItemDropListSet ClassifiedSet;

    // Iterate through all added item drop lists
    for (const auto &[Class, ItemDropsListSet] : ActiveUnsortedItemDropList)
    {
        // Get reference to current class item drop lists
        for (const TSoftObjectPtr<const UMTD_ItemDropList> ItemDropsList : ItemDropsListSet)
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

void UMTD_ItemDropSubsystem::SortActiveItemDropListsPool()
{
    // Sort gathered data
    for (const auto &[Class, ClassItemDropLists] : ActiveUnsortedItemDropList)
    {
        // Find max weight
        float SumWeight = 0.f;
        for (const TSoftObjectPtr<const UMTD_ItemDropList> ItemDropList : ClassItemDropLists)
        {
            // Every weight should be a non-negative number
            ensure(ItemDropList->Weight > 0.f);

            // Increase the sum
			SumWeight += ItemDropList->Weight;
        }

        // Create sorted item drop list container with normalized weights as keys
        SortedItemDropListSet ClassSortedItemDropLists;
        for (const TSoftObjectPtr<const UMTD_ItemDropList> ItemDropList : ClassItemDropLists)
        {
            const float NormalizedWeight = (ItemDropList->Weight / SumWeight);
            ClassSortedItemDropLists.Add(NormalizedWeight, ItemDropList);

            MTDS_VERBOSE("Class [%s] has received an item drop list [%s] with normalized weight [%f].",
                *Class->GetName(), *ItemDropList->GetName(), NormalizedWeight);
        }

        // Delete old values
        ActiveSortedItemDropList.Remove(Class);

        // Bind the computed data with character class
        ActiveSortedItemDropList.Add(Class, ClassSortedItemDropLists);
    }
}
