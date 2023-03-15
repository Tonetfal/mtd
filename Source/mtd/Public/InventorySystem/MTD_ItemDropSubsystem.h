#pragma once

#include "GameplayTagContainer.h"
#include "mtd.h"
#include "Subsystems/WorldSubsystem.h"

#include "MTD_ItemDropSubsystem.generated.h"

class AMTD_BaseFoeCharacter;

/**
 * Item drop list containing information about what to drop, to whom, and how often.
 *
 * All items inside the list have the equal drop probability, however, the random is defined by the list weight. The
 * higher the weight, the less likely the list will be selected among others.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_ItemDropList
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * List of classes the items will be dropped to. If at least one class is present, the items will be present in the
     * drop list. Otherwise, they will not.
     */
    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer HeroClasses;
    
    /**
     * List weight value. The higher the value, the less likely the list will be selected among others.
     * All items inside the list share the same chance.
     */
    UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0.0"))
    float Weight = 0.f;

    /** List of item IDs that can be dropped. */
    UPROPERTY(EditDefaultsOnly)
    TArray<int32> ItemIDs;
};

/**
 * Container of item drop lists.
 */
USTRUCT()
struct FMTD_ItemDropListSet
{
    GENERATED_BODY()

public:
    /** Item drop lists. */
    UPROPERTY(EditDefaultsOnly)
    TArray<TSoftObjectPtr<const UMTD_ItemDropList>> ItemDropsData;
};

/**
 * Subsystem containing possible item drops by various foes depending on present hero classes.
 *
 * It's intended to manage items foes drop on death in order to avoid giving items a player most likely doesn't need.
 * For instance, if a player uses a mage hero, he doesn't need nor swords, nor bows, but staffs, hence only item drop
 * lists that are designed for mages will be used, as well as the ones that are designed for all character, such as
 * armor drop lists.
 *
 * However, if a new hero class joins the game, then all it takes to enable item drops related to his class to active
 * item drop pool as well is a single AddHeroClass call.
 *
 * There are two main drop list pools:
 * - Regular pool, containing all the item drop lists that can be used by the manager. It includes both the classified,
 * and the broadcast pools.
 * - Active pool, containing all the item drop lists that are currently used to randomize items. They are dynamically
 * changed as different hero classes join and abandon the game.
 */
UCLASS()
class MTD_API UMTD_ItemDropSubsystem
    : public UWorldSubsystem
{
    GENERATED_BODY()

private:
    using ItemPtr = TSoftObjectPtr<const UMTD_ItemDropList>;
    using CharacterClass = TSoftClassPtr<AMTD_BaseFoeCharacter>;
    
    using UnsortedItemDropListSet = TArray<ItemPtr>;
    using ClassifiedUnsortedItemDropListSet = TMap<CharacterClass, UnsortedItemDropListSet>;

    // Container is sorted in ascending order
    using SortedItemDropListSet = TSortedMap<float /* Normalized Weight */, ItemPtr /* Item Drop List */>;
    using ClassifiedSortedItemDropListSet = TMap<CharacterClass, SortedItemDropListSet>;

public:
    /**
     * Get the subsystem from a world context.
     * @param   WorldContextObject: context used to retrieve the subsystem.
     * @return  This subsystem. May be nullptr.
     */
    static UMTD_ItemDropSubsystem *Get(const UObject *WorldContextObject);
    
    //~UWorldSubsystem Interface
    /**
     * Initialize item drop manager. Setup broadcast drop lists, and item pool.
     */
	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
    //~End of UWorldSubsystem Interface
    
    /**
     * Initialize this subsystem using item drop settings data.
     * @see UMTD_ItemDropSettings
     */
    void ReadSettings();
    
    /**
     * Get a random item drop list for a specific foe class.
     * @param   Class: foe class to search item drop list associated with.
     * @param   OutItemDropList: output parameter. Randomized item drop list.
     * @return  If true, item drop list has been randomized successfully, false otherwise.
     */
    bool GetRandomItemDropList(TSoftClassPtr<AMTD_BaseFoeCharacter> Class,
        const UMTD_ItemDropList **OutItemDropList) const;

    /**
     * Get a random item ID for a specific foe class.
     * @param   Class: foe class to search item ID for.
     * @param   OutItemID: output parameter. Randomized item ID.
     * @return  If true, item ID has been randomized successfully, false otherwise.
     */
    bool GetRandomItemID(TSoftClassPtr<AMTD_BaseFoeCharacter> Class, int32 &OutItemID) const;

    /**
     * Add an item drop list to item pool to all foe classes.
     * @param   ItemDropList: item drop list to add.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void AddListInPoolBroadcast(const UMTD_ItemDropList *ItemDropList, bool bSort = true);
    
    /**
     * Add an item drop list to item pool to a specific foe class.
     * @param   Class: foe class that the item drop list is associated with.
     * @param   ItemDropList: item drop list to add.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void AddListInPool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);
    
    /**
     * Add all item drop lists to all their associated foes.
     * @param   InItemDropLists: item drop lists to add to their associated foes.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void AddClassifiedListsInPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists, bool bSort = true);
    
    /**
     * Remove an item drop list from item pool from a specific foe class.
     * @param   Class: foe class that the item drop list is associated with.
     * @param   ItemDropList: item drop list to remove.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void RemoveListInPool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);
    
    /**
     * Remove all item drop lists from all their associated foes.
     * @param   InItemDropLists: item drop lists to remove from their associated foes.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void RemoveClassifiedListsFromPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists, bool bSort = true);

    /**
     * Add a new hero class to hero pool.
     * @param   HeroClass: hero class to add.
     */
    void AddHeroClass(const FGameplayTag &HeroClass);
    
    /**
     * Add new hero classes to hero pool.
     * @param   HeroClasses: hero classes to add.
     */
    void AddHeroClasses(const FGameplayTagContainer &HeroClasses);
    
    /**
     * Remove a hero class from hero pool.
     * @param   HeroClass: hero class to remove.
     */
    void RemoveHeroClass(const FGameplayTag &HeroClass);
    
    /**
     * Remove hero classes from hero pool.
     * @param   HeroClasses: hero classes to remove.
     */
    void RemoveHeroClasses(const FGameplayTagContainer &HeroClasses);

private:
    /**
     * Active an item drop list for a specific foe.
     * @param   Class: foe class that the item drop list is associated with.
     * @param   ItemDropList: item drop list to activate.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void AddListInActivePool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);
    
    /**
     * Activate item drop lists for specific foes.
     * @param   InDropItemLists: item drop lists to activate for their associated foes.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void AddListsInActivePool(ClassifiedUnsortedItemDropListSet &InDropItemLists, bool bSort = true);

    /**
     * Deactivate an item drop list for a specific foe.
     * @param   Class: foe class that the item drop list is associated with.
     * @param   ItemDropList: item drop list to deactivate.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void RemoveListFromActivePool(TSoftClassPtr<AMTD_BaseFoeCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);

    /**
     * Deactivate item drop lists for specific foes.
     * @param   InDropItemLists: drop item lists to deactivate for their associated foes.
     * @param   bSort: if true, active item drops pool will be sorted, false otherwise.
     */
    void RemoveListsFromActivePool(ClassifiedUnsortedItemDropListSet &InDropItemLists, bool bSort = true);

    /**
     * Activate item drop lists for new hero classes.
     * @param   AddedHeroClasses: new hero classes to activate new item drop lists for.
     */
    void AddListsToActivePoolForClasses(const FGameplayTagContainer &AddedHeroClasses);
    
    /**
     * Deactivate item drop lists from removed hero classes.
     * @param   RemovedHeroClasses: removed hero classes to deactivate item drop lists for.
     */
    void RemoveListsFromActivePoolForClasses(const FGameplayTagContainer &RemovedHeroClasses);

    /**
     * Sort active item drop lists pool.
     *
     * @see ActiveSortedItemDropList, ActiveUnsortedItemDropList
     */
    void SortActiveItemDropListsPool();

private:
    /** Chance an item can be dropped with. */
    UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0.0", ClampMax="100.0"))
    float DropChance = 30.f;

    /** Classified item drop lists pool to select from when adding a new hero class to the list. */
    UPROPERTY(EditDefaultsOnly, Category="Targetted")
    TMap<TSoftClassPtr<AMTD_BaseFoeCharacter>, FMTD_ItemDropListSet> ClassifiedItemDropListPool;

    /**
     * Enemy characters that listen for broadcasted item drop lists.
     * 
     * @see BroadcastItemDropLists
     */
    UPROPERTY(EditDefaultsOnly, Category="Broadcast")
    TArray<TSoftClassPtr<AMTD_BaseFoeCharacter>> BroadcastListeners;
    
    /**
     * Item drop lists pool to select from when adding a new hero class to the list.
     * 
     * If a list is matching with at least one of the hero classes, all the broadcast listeners will receive it.
     * 
     * @see BroadcastListeners
     */
    UPROPERTY(EditDefaultsOnly, Category="Broadcast")
    TArray<TSoftObjectPtr<const UMTD_ItemDropList>> BroadcastItemDropListPool;

    /** Hero classes that are present in current game instance. */
    FGameplayTagContainer ActiveHeroClasses;

    /** Item drop lists that are currently active, hence used to determine drops, which are classified, but unsorted. */
    ClassifiedUnsortedItemDropListSet ActiveUnsortedItemDropList;

    /**
     * Item drop lists that are currently active, hence used to determine drops, which are both classified and sorted to
     * ease the search.
     */
    ClassifiedSortedItemDropListSet ActiveSortedItemDropList;
};
