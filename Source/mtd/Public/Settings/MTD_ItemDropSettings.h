#pragma once

#include "Engine/DeveloperSettings.h"
#include "InventorySystem/MTD_ItemDropSubsystem.h"
#include "mtd.h"

#include "MTD_ItemDropSettings.generated.h"

class AMTD_BaseFoeCharacter;

/**
 * Settings containing all enemy drop data.
 */
UCLASS(Config="Game", DefaultConfig, DisplayName="Item Drops")
class MTD_API UMTD_ItemDropSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /**
     * Get singleton instance of item drop settings.
     * @return  Singleton instance of item drop settings.
     */
	static UMTD_ItemDropSettings *Get();

    //~UDeveloperSettings Interface
    virtual FName GetCategoryName() const override;
    //~End of UDeveloperSettings Interface

private:
    /** Singleton instance of the item drop settings. */
    inline static TObjectPtr<UMTD_ItemDropSettings> DefaultSettings = nullptr;

public:
    /** Chance an item can be dropped with. */
    UPROPERTY(Config, EditAnywhere, meta=(ClampMin="0.0", ClampMax="100.0"))
    float DropChance = 30.f;

    /** Classified item drop lists pool to select from when adding a new hero class to the list. */
    UPROPERTY(Config, EditAnywhere, Category="Targetted")
    TMap<TSoftClassPtr<AMTD_BaseFoeCharacter>, FMTD_ItemDropListSet> ClassifiedItemDropListPool;
    
    /**
     * Enemy characters that listen for broadcasted item drop lists.
     * 
     * @see BroadcastItemDropLists
     */
    UPROPERTY(Config, EditAnywhere, Category="Broadcast")
    TArray<TSoftClassPtr<AMTD_BaseFoeCharacter>> BroadcastListeners;
    
    /**
     * Item drop lists pool to select from when adding a new hero class to the list.
     * 
     * If a list is matching with at least one of the hero classes, all the broadcast listeners will receive it.
     * 
     * @see BroadcastListeners
     */
    UPROPERTY(Config, EditAnywhere, Category="Broadcast")
    TArray<TSoftObjectPtr<const UMTD_ItemDropList>> BroadcastItemDropListPool;
};

inline FName UMTD_ItemDropSettings::GetCategoryName() const
{
    return TEXT("MTD");
}
