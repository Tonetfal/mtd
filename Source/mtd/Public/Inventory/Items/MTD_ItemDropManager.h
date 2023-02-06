#pragma once

#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_ItemDropManager.generated.h"

class AMTD_BaseEnemyCharacter;

UCLASS(BlueprintType, Const)
class MTD_API UMTD_ItemDropList
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * List of classes the items will be dropped to. If none of the classes are present, the items will not be in the
     * drop list.
     */
    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer HeroClasses;
    
    /** */
    UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0.0"))
    float Weight = 0.f;

    /** List of item IDs that can be dropped from this list. */
    UPROPERTY(EditDefaultsOnly)
    TArray<int32> ItemIDs;
};

USTRUCT()
struct FMTD_ItemDropListSet
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TArray<TObjectPtr<const UMTD_ItemDropList>> ItemDropsData;
};

/**
 * Singleton containing possibly item drops by various enemies.
 */
UCLASS(Blueprintable)
class MTD_API UMTD_ItemDropManager
    : public UObject
{
    GENERATED_BODY()
    
private:
    friend class UMTD_GameInstance;

private:
    using ItemPtr = TObjectPtr<const UMTD_ItemDropList>;
    using CharacterClass = TSubclassOf<AMTD_BaseEnemyCharacter>;
    
    using UnsortedItemDropListSet = TArray<ItemPtr>;
    using ClassifiedUnsortedItemDropListSet = TMap<CharacterClass, UnsortedItemDropListSet>;
    
    using SortedItemDropListSet = TSortedMap<float /* Normilized Weight */, ItemPtr /* Item Drop List */>;
    using ClassifiedSortedItemDropListSet = TMap<CharacterClass, SortedItemDropListSet>;

private:
    UMTD_ItemDropManager();
    void Init();
    
    static UMTD_ItemDropManager *Construct();
    static void Destroy();
    
public:
    static UMTD_ItemDropManager *Get();

public:
    void AddListInPoolBroadcast(const UMTD_ItemDropList *ItemDropList, bool bSort = true);
    void AddListInPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);
    void AddClassifiedListsInPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists, bool bSort = true);
    
    void RemoveListInPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);
    void RemoveClassifiedListsFromPool(const ClassifiedUnsortedItemDropListSet &InItemDropLists, bool bSort = true);
    
    void AddHeroClass(const FGameplayTag &HeroClass);
    void AddHeroClasses(const FGameplayTagContainer &HeroClasses);
    
    void RemoveHeroClass(const FGameplayTag &HeroClass);
    void RemoveHeroClasses(const FGameplayTagContainer &HeroClasses);

private:
    void AddListInGlobalPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);
    void AddListsInGlobalPool(ClassifiedUnsortedItemDropListSet &ClassifiedUnsortedSet, bool bSort = true);
    
    void RemoveListFromGlobalPool(TSubclassOf<AMTD_BaseEnemyCharacter> Class, const UMTD_ItemDropList *ItemDropList,
        bool bSort = true);
    void RemoveListsFromGlobalPool(ClassifiedUnsortedItemDropListSet &RemovedItems, bool bSort = true);
    
    void AddListsToGlobalDropsForClasses(const FGameplayTagContainer &AddedHeroClasses);
    void RemoveListsFromGlobalDropsForClasses(const FGameplayTagContainer &RemovedHeroClasses);
    
    void SortGlobalItemDropLists();

public:
    bool GetRandomItemDropList(TSubclassOf<AMTD_BaseEnemyCharacter> Class,
        const UMTD_ItemDropList **OutItemDropList) const;
    bool GetRandomItemID(TSubclassOf<AMTD_BaseEnemyCharacter> Class, int32 &OutItemID) const;

private:
    /** Singleton instance of the Item Drop Manager. */
    inline static TObjectPtr<UMTD_ItemDropManager> ItemDropManager;

    /** Chance an item can be dropped with. */
    UPROPERTY(EditDefaultsOnly, meta=(ClampMin="0.0", ClampMax="100.0"))
    float DropChance = 30.f;

    /** Classified Item Drop Lists pool to select from when adding a new hero class to the list. */
    UPROPERTY(EditDefaultsOnly, Category="Targetted")
    TMap<TSubclassOf<AMTD_BaseEnemyCharacter>, FMTD_ItemDropListSet> ClassifiedItemDropListPool;

    /**
     * Enemy characters that listen for broadcasted Item Drop Lists.
     * @see BroadcastItemDropLists
     */
    UPROPERTY(EditDefaultsOnly, Category="Broadcast")
    TArray<TSubclassOf<AMTD_BaseEnemyCharacter>> BroadcastListeners;
    
    /**
     * Item Drop Lists pool to select from when adding a new hero class to the list.
     * 
     * If a list is matching with hero classes, all the broadcast listeners will receive it.
     * @see BroadcastListeners
     */
    UPROPERTY(EditDefaultsOnly, Category="Broadcast")
    TArray<TObjectPtr<const UMTD_ItemDropList>> BroadcastItemDropListPool;

    /** Hero classes that are present on the level. */
    FGameplayTagContainer ActiveHeroClasses;

    /** Item Drop Lists that are used to determine drops, which are classified, but unsorted. */
    ClassifiedUnsortedItemDropListSet GlobalUnsortedItemDropList;

    /** Item Drop Lists that are used to determine drops, which are both classified and sorted to ease the search. */
    ClassifiedSortedItemDropListSet GlobalSortedItemDropList;
};
